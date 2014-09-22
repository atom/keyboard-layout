#ifndef SRC_KEYBORD_LAYOUT_OBSERVER_H_
#define SRC_KEYBORD_LAYOUT_OBSERVER_H_

#include "nan.h"

using namespace v8;  // NOLINT

class KeyboardLayoutObserver : public node::ObjectWrap {
 public:
  static void Init(Handle<Object> target);
  void HandleKeyboardLayoutChanged();

 private:
  KeyboardLayoutObserver(NanCallback *callback);
  ~KeyboardLayoutObserver();
  static NAN_METHOD(New);
  static NAN_METHOD(GetCurrentKeyboardLayout);

  NanCallback *callback;
};

#endif  // SRC_KEYBORD_LAYOUT_OBSERVER_H_
