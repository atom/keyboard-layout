#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601

#undef WINVER
#define WINVER 0x0601

#include "keyboard-layout-observer.h"

#include <string>
#include <windows.h>

using namespace v8;

std::string ToUTF8(const std::wstring& string) {
  if (string.length() < 1) {
    return std::string();
  }

  // NB: In the pathological case, each character could expand up
  // to 4 bytes in UTF8.
  int cbLen = (string.length()+1) * sizeof(char) * 4;
  char* buf = new char[cbLen];
  int retLen = WideCharToMultiByte(CP_UTF8, 0, string.c_str(), string.length(), buf, cbLen, NULL, NULL);
  buf[retLen] = 0;

  std::string ret;
  ret.assign(buf);
  return ret;
}

std::wstring ToWString(const std::string& string) {
  if (string.length() < 1) {
    return std::wstring();
  }

  // NB: If you got really unlucky, every character could be a two-wchar_t
  // surrogate pair
  int cchLen = (string.length()+1) * 2;
  wchar_t* buf = new wchar_t[cchLen];
  int retLen = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), strlen(string.c_str()), buf, cchLen);
  buf[retLen] = 0;

  std::wstring ret;
  ret.assign(buf);
  return ret;
}

void KeyboardLayoutObserver::Init(Handle<Object> target) {
  NanScope();
  Local<FunctionTemplate> newTemplate = NanNew<FunctionTemplate>(KeyboardLayoutObserver::New);
  newTemplate->SetClassName(NanNew<String>("KeyboardLayoutObserver"));
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Local<ObjectTemplate> proto = newTemplate->PrototypeTemplate();
  NODE_SET_METHOD(proto, "getCurrentKeyboardLayout", KeyboardLayoutObserver::GetCurrentKeyboardLayout);
  target->Set(NanNew<String>("KeyboardLayoutObserver"), newTemplate->GetFunction());
}

NODE_MODULE(keyboard_layout_observer, KeyboardLayoutObserver::Init)

NAN_METHOD(KeyboardLayoutObserver::New) {
  NanScope();

  Local<Function> callbackHandle = args[0].As<Function>();
  NanCallback *callback = new NanCallback(callbackHandle);

  KeyboardLayoutObserver *observer = new KeyboardLayoutObserver(callback);
  observer->Wrap(args.This());
  NanReturnUndefined();
}

KeyboardLayoutObserver::KeyboardLayoutObserver(NanCallback *callback) : callback(callback) {
}

KeyboardLayoutObserver::~KeyboardLayoutObserver() {
  delete callback;
};

void KeyboardLayoutObserver::HandleKeyboardLayoutChanged() {
}

NAN_METHOD(KeyboardLayoutObserver::GetCurrentKeyboardLayout) {
  NanScope();

  char layoutName[KL_NAMELENGTH];
  if (::GetKeyboardLayoutName(layoutName))
    NanReturnValue(NanNew(layoutName));
  else
    NanReturnValue(NanUndefined());
}
