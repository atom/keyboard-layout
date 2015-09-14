#ifndef SRC_KEYBORD_LAYOUT_OBSERVER_H_
#define SRC_KEYBORD_LAYOUT_OBSERVER_H_

#include "nan.h"

using namespace v8;  // NOLINT

class KeyboardLayoutObserver : public Nan::ObjectWrap {
 public:
  static void Init(Handle<Object> target);
  void HandleKeyboardLayoutChanged();

 private:
  KeyboardLayoutObserver(Nan::Callback *callback);
  ~KeyboardLayoutObserver();
  static NAN_METHOD(New);
  static NAN_METHOD(GetCurrentKeyboardLayout);
  static NAN_METHOD(GetCurrentKeyboardLanguage);
  static NAN_METHOD(GetInstalledKeyboardLanguages);

  Nan::Callback *callback;
};

#endif  // SRC_KEYBORD_LAYOUT_OBSERVER_H_
