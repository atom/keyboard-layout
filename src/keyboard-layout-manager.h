#ifndef SRC_KEYBORD_LAYOUT_OBSERVER_H_
#define SRC_KEYBORD_LAYOUT_OBSERVER_H_

#include "nan.h"

using namespace v8;  // NOLINT

class KeyboardLayoutManager : public Nan::ObjectWrap {
 public:
  static void Init(Handle<Object> target, Handle<Object> module);
  void HandleKeyboardLayoutChanged();

 private:
  KeyboardLayoutManager(Nan::Callback *callback);
  ~KeyboardLayoutManager();
  static NAN_METHOD(New);
  static NAN_METHOD(GetCurrentKeyboardLayout);
  static NAN_METHOD(GetCurrentKeyboardLanguage);
  static NAN_METHOD(GetInstalledKeyboardLanguages);
  static NAN_METHOD(GetCurrentKeymap);

  Nan::Callback *callback;
};

#endif  // SRC_KEYBORD_LAYOUT_OBSERVER_H_
