#import <Cocoa/Cocoa.h>
#import <dispatch/dispatch.h>

#import <Carbon/Carbon.h>
#import "keyboard-layout-manager.h"
#include <vector>
#include <string>
#include <cctype>

NAN_MODULE_INIT(KeyboardLayoutManager::Init) {
  Nan::HandleScope scope;
  v8::Local<v8::FunctionTemplate> newTemplate = Nan::New<v8::FunctionTemplate>(KeyboardLayoutManager::New);
  newTemplate->SetClassName(Nan::New<v8::String>("KeyboardLayoutManager").ToLocalChecked());
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);

  v8::Local<v8::ObjectTemplate> proto = newTemplate->PrototypeTemplate();

  Nan::SetMethod(proto, "getCurrentKeyboardLayout", KeyboardLayoutManager::GetCurrentKeyboardLayout);
  Nan::SetMethod(proto, "getCurrentKeyboardLanguage", KeyboardLayoutManager::GetCurrentKeyboardLanguage);
  Nan::SetMethod(proto, "getInstalledKeyboardLanguages", KeyboardLayoutManager::GetInstalledKeyboardLanguages);
  Nan::SetMethod(proto, "getCurrentKeymap", KeyboardLayoutManager::GetCurrentKeymap);

  Nan::Set(target, Nan::New("exports").ToLocalChecked(), Nan::GetFunction(newTemplate).ToLocalChecked());
}

NODE_MODULE(keyboard_layout_manager, KeyboardLayoutManager::Init)

NAN_METHOD(KeyboardLayoutManager::New) {
  Nan::HandleScope scope;

  v8::Local<v8::Function> callbackHandle = info[0].As<v8::Function>();
  Nan::Callback *callback = new Nan::Callback(callbackHandle);

  KeyboardLayoutManager *manager = new KeyboardLayoutManager(callback);
  manager->Wrap(info.This());
  return;
}

uv_loop_t *loop = uv_default_loop();
uv_async_t async;

static void notificationHandler(CFNotificationCenterRef center, void *manager, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
  async.data = manager;
  uv_async_send(&async);
}

static void asyncSendHandler(uv_async_t *handle) {
  (static_cast<KeyboardLayoutManager *>(handle->data))->HandleKeyboardLayoutChanged();
}

KeyboardLayoutManager::KeyboardLayoutManager(Nan::Callback *callback) : callback(callback) {
  uv_async_init(loop, &async, (uv_async_cb) asyncSendHandler);

  CFNotificationCenterAddObserver(
      CFNotificationCenterGetDistributedCenter(),
      this,
      &notificationHandler,
      kTISNotifySelectedKeyboardInputSourceChanged,
      NULL,
      CFNotificationSuspensionBehaviorDeliverImmediately
  );
}

KeyboardLayoutManager::~KeyboardLayoutManager() {
  delete callback;
};

void KeyboardLayoutManager::HandleKeyboardLayoutChanged() {
  Nan::Call(*callback, 0, NULL);
}

NAN_METHOD(KeyboardLayoutManager::GetInstalledKeyboardLanguages) {
  Nan::HandleScope scope;

  @autoreleasepool {
    std::vector<std::string> ret;

    // NB: We have to do this whole rigamarole twice, once for IMEs (i.e.
    // Japanese), and once for keyboard layouts (i.e. English).
    NSDictionary* filter = @{ (__bridge NSString *) kTISPropertyInputSourceType : (__bridge NSString *) kTISTypeKeyboardLayout };
    NSArray* keyboardLayouts = (NSArray *) TISCreateInputSourceList((__bridge CFDictionaryRef) filter, NO);

    for (size_t i=0; i < keyboardLayouts.count; i++) {
      TISInputSourceRef current = (TISInputSourceRef)[keyboardLayouts objectAtIndex:i];

      NSArray* langs = (NSArray*) TISGetInputSourceProperty(current, kTISPropertyInputSourceLanguages);
      std::string str = std::string([(NSString*)[langs objectAtIndex:0] UTF8String]);
      ret.push_back(str);
    }

    filter = @{ (__bridge NSString *) kTISPropertyInputSourceType : (__bridge NSString *) kTISTypeKeyboardInputMode };
    keyboardLayouts = (NSArray *) TISCreateInputSourceList((__bridge CFDictionaryRef) filter, NO);

    for (size_t i=0; i < keyboardLayouts.count; i++) {
      TISInputSourceRef current = (TISInputSourceRef)[keyboardLayouts objectAtIndex:i];

      NSArray* langs = (NSArray*) TISGetInputSourceProperty(current, kTISPropertyInputSourceLanguages);
      std::string str = std::string([(NSString*)[langs objectAtIndex:0] UTF8String]);
      ret.push_back(str);
    }

    v8::Local<v8::Array> result = Nan::New<v8::Array>(ret.size());
    for (size_t i = 0; i < ret.size(); ++i) {
       const std::string& lang = ret[i];
       Nan::Set(result, i, Nan::New<v8::String>(lang.data(), lang.size()).ToLocalChecked());
    }

    info.GetReturnValue().Set(result);
  }
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeyboardLanguage) {
  Nan::HandleScope scope;
  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();

  NSArray* langs = (NSArray*) TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages);
  NSString* lang = (NSString*) [langs objectAtIndex:0];

  info.GetReturnValue().Set(Nan::New([lang UTF8String]).ToLocalChecked());
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeyboardLayout) {
  Nan::HandleScope scope;
  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFStringRef sourceId = (CFStringRef) TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
  info.GetReturnValue().Set(Nan::New([(NSString *)sourceId UTF8String]).ToLocalChecked());
}

struct KeycodeMapEntry {
  UInt16 virtualKeyCode;
  const char *dom3Code;
};

#define USB_KEYMAP_DECLARATION static const KeycodeMapEntry keyCodeMap[] =
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) {mac, code}

#include "keycode_converter_data.inc"

v8::Local<v8::Value> CharacterForNativeCode(const UCKeyboardLayout* keyboardLayout, UInt16 virtualKeyCode, EventModifiers modifiers) {
  // See https://developer.apple.com/reference/coreservices/1390584-uckeytranslate?language=objc
  UInt32 modifierKeyState = (modifiers >> 8) & 0xFF;
  UInt32 deadKeyState = 0;
  UniChar characters[4] = {0, 0, 0, 0};
  UniCharCount charCount = 0;
  OSStatus status = UCKeyTranslate(
      keyboardLayout,
      static_cast<UInt16>(virtualKeyCode),
      kUCKeyActionDown,
      modifierKeyState,
      LMGetKbdLast(),
      kUCKeyTranslateNoDeadKeysBit,
      &deadKeyState,
      sizeof(characters) / sizeof(characters[0]),
      &charCount,
      characters);

  // If the previous key was dead, translate again with the same dead key
  // state to get a printable character.
  if (status == noErr && deadKeyState != 0) {
    status = UCKeyTranslate(
        keyboardLayout,
        static_cast<UInt16>(virtualKeyCode),
        kUCKeyActionDown,
        modifierKeyState,
        LMGetKbdLast(),
        kUCKeyTranslateNoDeadKeysBit,
        &deadKeyState,
        sizeof(characters) / sizeof(characters[0]),
        &charCount,
        characters);
  }

  if (status == noErr && !std::iscntrl(characters[0])) {
    return Nan::New(static_cast<const uint16_t *>(characters), static_cast<int>(charCount)).ToLocalChecked();
  } else {
    return Nan::Null();
  }
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeymap) {
  TISInputSourceRef source = TISCopyCurrentKeyboardLayoutInputSource();
  CFDataRef layoutData = static_cast<CFDataRef>(TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData));

  if (layoutData == NULL) {
    info.GetReturnValue().Set(Nan::Null());
    return;
  }

  const UCKeyboardLayout* keyboardLayout = reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layoutData));

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  v8::Local<v8::String> unmodifiedKey = Nan::New("unmodified").ToLocalChecked();
  v8::Local<v8::String> withShiftKey = Nan::New("withShift").ToLocalChecked();
  v8::Local<v8::String> withAltGraphKey = Nan::New("withAltGraph").ToLocalChecked();
  v8::Local<v8::String> withAltGraphShiftKey = Nan::New("withAltGraphShift").ToLocalChecked();

  size_t keyCodeMapSize = sizeof(keyCodeMap) / sizeof(keyCodeMap[0]);
  for (size_t i = 0; i < keyCodeMapSize; i++) {
    const char *dom3Code = keyCodeMap[i].dom3Code;
    int virtualKeyCode = keyCodeMap[i].virtualKeyCode;
    if (dom3Code && virtualKeyCode < 0xffff) {
      v8::Local<v8::String> dom3CodeKey = Nan::New(dom3Code).ToLocalChecked();

      v8::Local<v8::Value> unmodified = CharacterForNativeCode(keyboardLayout, virtualKeyCode, 0);
      v8::Local<v8::Value> withShift = CharacterForNativeCode(keyboardLayout, virtualKeyCode, (1 << shiftKeyBit));
      v8::Local<v8::Value> withAltGraph = CharacterForNativeCode(keyboardLayout, virtualKeyCode, (1 << optionKeyBit));
      v8::Local<v8::Value> withAltGraphShift = CharacterForNativeCode(keyboardLayout, virtualKeyCode, (1 << shiftKeyBit) | (1 << optionKeyBit));

      if (unmodified->IsString() || withShift->IsString() || withAltGraph->IsString() || withAltGraphShift->IsString()) {
        v8::Local<v8::Object> entry = Nan::New<v8::Object>();
        Nan::Set(entry, unmodifiedKey, unmodified);
        Nan::Set(entry, withShiftKey, withShift);
        Nan::Set(entry, withAltGraphKey, withAltGraph);
        Nan::Set(entry, withAltGraphShiftKey, withAltGraphShift);

        Nan::Set(result, dom3CodeKey, entry);
      }
    }
  }

  info.GetReturnValue().Set(result);
}
