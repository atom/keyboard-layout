#ifndef SRC_KEYBORD_LAYOUT_OBSERVER_H_
#define SRC_KEYBORD_LAYOUT_OBSERVER_H_

#include "nan.h"

#if defined(__linux__) || defined(__FreeBSD__)
#include <X11/Xlib.h>
#endif // __linux__ || __FreeBSD__

class KeyboardLayoutManager : public Nan::ObjectWrap {
 public:
  void HandleKeyboardLayoutChanged();

  static NAN_METHOD(New);
  static NAN_METHOD(GetCurrentKeyboardLayout);
  static NAN_METHOD(GetCurrentKeyboardLanguage);
  static NAN_METHOD(GetInstalledKeyboardLanguages);
  static NAN_METHOD(GetCurrentKeymap);

 private:
  KeyboardLayoutManager(v8::Isolate* isolate, Nan::Callback *callback);
  ~KeyboardLayoutManager();

  v8::Isolate* isolate() { return isolate_; }

#if defined(__linux__) || defined(__FreeBSD__)
  Display *xDisplay;
  XIC xInputContext;
  XIM xInputMethod;
#endif

  v8::Isolate *isolate_;
  Nan::Callback *callback;
};

#endif  // SRC_KEYBORD_LAYOUT_OBSERVER_H_
