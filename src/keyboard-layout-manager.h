#ifndef SRC_KEYBORD_LAYOUT_OBSERVER_H_
#define SRC_KEYBORD_LAYOUT_OBSERVER_H_

#include "nan.h"

#ifdef __linux__
#include <X11/Xlib.h>
#endif // __linux__

class KeyboardLayoutManager : public Nan::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target, v8::Handle<v8::Object> module);
  void HandleKeyboardLayoutChanged();

 private:
  KeyboardLayoutManager(Nan::Callback *callback);
  ~KeyboardLayoutManager();
  static NAN_METHOD(New);
  static NAN_METHOD(GetCurrentKeyboardLayout);
  static NAN_METHOD(GetCurrentKeyboardLanguage);
  static NAN_METHOD(GetInstalledKeyboardLanguages);
  static NAN_METHOD(GetCurrentKeymap);

#ifdef __linux__
  Display *xDisplay;
  XIC xInputContext;
  XIM xInputMethod;
#endif

  Nan::Callback *callback;
};

#endif  // SRC_KEYBORD_LAYOUT_OBSERVER_H_
