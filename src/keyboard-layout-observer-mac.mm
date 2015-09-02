#import <Cocoa/Cocoa.h>
#import <dispatch/dispatch.h>

#import <Carbon/Carbon.h>
#import "keyboard-layout-observer.h"
#include <vector>
#include <string>

using namespace v8;

void KeyboardLayoutObserver::Init(Handle<Object> target) {
  Nan::HandleScope scope;
  Local<FunctionTemplate> newTemplate = Nan::New<FunctionTemplate>(KeyboardLayoutObserver::New);
  newTemplate->SetClassName(Nan::New<String>("KeyboardLayoutObserver").ToLocalChecked());
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> proto = newTemplate->PrototypeTemplate();

  Nan::SetMethod(proto, "getCurrentKeyboardLayout", KeyboardLayoutObserver::GetCurrentKeyboardLayout);
  Nan::SetMethod(proto, "getCurrentKeyboardLanguage", KeyboardLayoutObserver::GetCurrentKeyboardLanguage);
  Nan::SetMethod(proto, "getInstalledKeyboardLanguages", KeyboardLayoutObserver::GetInstalledKeyboardLanguages);

  target->Set(Nan::New<String>("KeyboardLayoutObserver").ToLocalChecked(), newTemplate->GetFunction());
}

NODE_MODULE(keyboard_layout_observer, KeyboardLayoutObserver::Init)

NAN_METHOD(KeyboardLayoutObserver::New) {
  Nan::HandleScope scope;

  Local<Function> callbackHandle = info[0].As<Function>();
  Nan::Callback *callback = new Nan::Callback(callbackHandle);

  KeyboardLayoutObserver *observer = new KeyboardLayoutObserver(callback);
  observer->Wrap(info.This());
  return;
}

uv_loop_t *loop = uv_default_loop();
uv_async_t async;

static void notificationHandler(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
  async.data = observer;
  uv_async_send(&async);
}

static void asyncSendHandler(uv_async_t *handle) {
  (static_cast<KeyboardLayoutObserver *>(handle->data))->HandleKeyboardLayoutChanged();
}

KeyboardLayoutObserver::KeyboardLayoutObserver(Nan::Callback *callback) : callback(callback) {
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

KeyboardLayoutObserver::~KeyboardLayoutObserver() {
  delete callback;
};

void KeyboardLayoutObserver::HandleKeyboardLayoutChanged() {
  callback->Call(0, NULL);
}

NAN_METHOD(KeyboardLayoutObserver::GetInstalledKeyboardLanguages) {
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

    Local<Array> result = Nan::New<Array>(ret.size());
    for (size_t i = 0; i < ret.size(); ++i) {
       const std::string& lang = ret[i];
       result->Set(i, Nan::New<String>(lang.data(), lang.size())).ToLocalChecked();
    }

    info.GetReturnValue().Set(result);
  }
}

NAN_METHOD(KeyboardLayoutObserver::GetCurrentKeyboardLanguage) {
  Nan::HandleScope scope;
  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();

  NSArray* langs = (NSArray*) TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages);
  NSString* lang = (NSString*) [langs objectAtIndex:0];

  info.GetReturnValue().Set(Nan::New([lang UTF8String]));
}

NAN_METHOD(KeyboardLayoutObserver::GetCurrentKeyboardLayout) {
  Nan::HandleScope scope;
  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFStringRef sourceId = (CFStringRef) TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
  info.GetReturnValue().Set(Nan::New([(NSString *)sourceId UTF8String]));
}
