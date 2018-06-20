#include "keyboard-layout-manager.h"

#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XKBrules.h>
#include <cwctype>
#include <cctype>

void KeyboardLayoutManager::Init(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module) {
  Nan::HandleScope scope;
  v8::Local<v8::FunctionTemplate> newTemplate = Nan::New<v8::FunctionTemplate>(KeyboardLayoutManager::New);
  newTemplate->SetClassName(Nan::New<v8::String>("KeyboardLayoutManager").ToLocalChecked());
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  v8::Local<v8::ObjectTemplate> proto = newTemplate->PrototypeTemplate();
  Nan::SetMethod(proto, "getCurrentKeyboardLayout", KeyboardLayoutManager::GetCurrentKeyboardLayout);
  Nan::SetMethod(proto, "getCurrentKeyboardLanguage", KeyboardLayoutManager::GetCurrentKeyboardLayout); // NB:  Intentionally mapped to same stub
  Nan::SetMethod(proto, "getInstalledKeyboardLanguages", KeyboardLayoutManager::GetInstalledKeyboardLanguages);
  Nan::SetMethod(proto, "getCurrentKeymap", KeyboardLayoutManager::GetCurrentKeymap);
  module->Set(Nan::New("exports").ToLocalChecked(), newTemplate->GetFunction());
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

KeyboardLayoutManager::KeyboardLayoutManager(Nan::Callback *callback) : xInputContext{nullptr}, xInputMethod{nullptr}, callback{callback} {
  xDisplay = XOpenDisplay("");
  if (!xDisplay) {
    Nan::ThrowError("Could not connect to X display.");
    return;
  }

  xInputMethod = XOpenIM(xDisplay, 0, 0, 0);
  if (!xInputMethod) {
    return;
  }

  XIMStyles* styles = 0;
  if (XGetIMValues(xInputMethod, XNQueryInputStyle, &styles, NULL) || !styles) {
    return;
  }

  XIMStyle bestMatchStyle = 0;
  for (int i = 0; i < styles->count_styles; i++) {
    XIMStyle thisStyle = styles->supported_styles[i];
    if (thisStyle == (XIMPreeditNothing | XIMStatusNothing))
    {
      bestMatchStyle = thisStyle;
      break;
    }
  }
  XFree(styles);
  if (!bestMatchStyle) {
    return;
  }

  Window window;
  int revert_to;
  XGetInputFocus(xDisplay, &window, &revert_to);
  if (window != BadRequest) {
    xInputContext = XCreateIC(
      xInputMethod, XNInputStyle, bestMatchStyle, XNClientWindow, window,
      XNFocusWindow, window, NULL
    );
  }
}

KeyboardLayoutManager::~KeyboardLayoutManager() {
  if (xInputContext) {
    XDestroyIC(xInputContext);
  }

  if (xInputMethod) {
    XCloseIM(xInputMethod);
  }

  XCloseDisplay(xDisplay);
  delete callback;
};

void KeyboardLayoutManager::HandleKeyboardLayoutChanged() {
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeyboardLayout) {
  Nan::HandleScope scope;
  KeyboardLayoutManager* manager = Nan::ObjectWrap::Unwrap<KeyboardLayoutManager>(info.Holder());
  v8::Local<v8::Value> result;

  XkbRF_VarDefsRec vdr;
  char *tmp = NULL;
  if (XkbRF_GetNamesProp(manager->xDisplay, &tmp, &vdr) && vdr.layout) {
    XkbStateRec xkbState;
    XkbGetState(manager->xDisplay, XkbUseCoreKbd, &xkbState);
    if (vdr.variant) {
      result = Nan::New<v8::String>(std::string(vdr.layout) + "," + std::string(vdr.variant) + " [" + std::to_string(xkbState.group) + "]").ToLocalChecked();
    } else {
      result = Nan::New<v8::String>(std::string(vdr.layout) + " [" + std::to_string(xkbState.group) + "]").ToLocalChecked();
    }
  } else {
    result = Nan::Null();
  }

  info.GetReturnValue().Set(result);

  return;
}

NAN_METHOD(KeyboardLayoutManager::GetInstalledKeyboardLanguages) {
  Nan::HandleScope scope;
  return;
}

struct KeycodeMapEntry {
  uint xkbKeycode;
  const char *dom3Code;
};

#define USB_KEYMAP_DECLARATION static const KeycodeMapEntry keyCodeMap[] =
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) {xkb, code}

#include "keycode_converter_data.inc"

v8::Local<v8::Value> CharacterForNativeCode(XIC xInputContext, XKeyEvent *keyEvent, uint xkbKeycode, uint state) {
  keyEvent->keycode = xkbKeycode;
  keyEvent->state = state;

  if (xInputContext) {
    wchar_t characters[2];
    int count = XwcLookupString(xInputContext, keyEvent, characters, 2, NULL, NULL);
    if (count > 0 && !std::iswcntrl(characters[0])) {
      return Nan::New<v8::String>(reinterpret_cast<const uint16_t *>(characters), count).ToLocalChecked();
    } else {
      return Nan::Null();
    }
  } else {
    // Graceful fallback for systems where no window is open or no input context
    // can be found.
    char characters[2];
    int count = XLookupString(keyEvent, characters, 2, NULL, NULL);
    if (count > 0 && !std::iscntrl(characters[0])) {
      return Nan::New<v8::String>(reinterpret_cast<const uint16_t *>(characters), count).ToLocalChecked();
    } else {
      return Nan::Null();
    }
  }
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeymap) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  KeyboardLayoutManager* manager = Nan::ObjectWrap::Unwrap<KeyboardLayoutManager>(info.Holder());
  v8::Local<v8::String> unmodifiedKey = Nan::New("unmodified").ToLocalChecked();
  v8::Local<v8::String> withShiftKey = Nan::New("withShift").ToLocalChecked();

  // Clear cached keymap
  XMappingEvent eventMap = {MappingNotify, 0, false, manager->xDisplay, 0, MappingKeyboard, 0, 0};
  XRefreshKeyboardMapping(&eventMap);

  XkbStateRec xkbState;
  XkbGetState(manager->xDisplay, XkbUseCoreKbd, &xkbState);
  uint keyboardBaseState = 0x0000;
  if (xkbState.group == 1) {
    keyboardBaseState = 0x2000;
  } else if (xkbState.group == 2) {
    keyboardBaseState = 0x4000;
  }

  // Set up an event to reuse across CharacterForNativeCode calls
  XEvent event;
  memset(&event, 0, sizeof(XEvent));
  XKeyEvent* keyEvent = &event.xkey;
  keyEvent->display = manager->xDisplay;
  keyEvent->type = KeyPress;

  size_t keyCodeMapSize = sizeof(keyCodeMap) / sizeof(keyCodeMap[0]);
  for (size_t i = 0; i < keyCodeMapSize; i++) {
    const char *dom3Code = keyCodeMap[i].dom3Code;
    uint xkbKeycode = keyCodeMap[i].xkbKeycode;

    if (dom3Code && xkbKeycode > 0x0000) {
      v8::Local<v8::String> dom3CodeKey = Nan::New(dom3Code).ToLocalChecked();
      v8::Local<v8::Value> unmodified = CharacterForNativeCode(manager->xInputContext, keyEvent, xkbKeycode, keyboardBaseState);
      v8::Local<v8::Value> withShift = CharacterForNativeCode(manager->xInputContext, keyEvent, xkbKeycode, keyboardBaseState | ShiftMask);

      if (unmodified->IsString() || withShift->IsString()) {
        v8::Local<v8::Object> entry = Nan::New<v8::Object>();
        entry->Set(unmodifiedKey, unmodified);
        entry->Set(withShiftKey, withShift);
        result->Set(dom3CodeKey, entry);
      }
    }
  }

  info.GetReturnValue().Set(result);
}
