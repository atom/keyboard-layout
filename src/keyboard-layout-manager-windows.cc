#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601

#undef WINVER
#define WINVER 0x0601

#include "keyboard-layout-manager.h"

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

void KeyboardLayoutManager::Init(Handle<Object> exports, Handle<Object> module) {
  Nan::HandleScope scope;
  Local<FunctionTemplate> newTemplate = Nan::New<FunctionTemplate>(KeyboardLayoutManager::New);
  newTemplate->SetClassName(Nan::New<String>("KeyboardLayoutManager").ToLocalChecked());
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Local<ObjectTemplate> proto = newTemplate->PrototypeTemplate();

  Nan::SetMethod(proto, "getCurrentKeyboardLayout", KeyboardLayoutManager::GetCurrentKeyboardLayout);
  Nan::SetMethod(proto, "getCurrentKeyboardLanguage", KeyboardLayoutManager::GetCurrentKeyboardLanguage);
  Nan::SetMethod(proto, "getInstalledKeyboardLanguages", KeyboardLayoutManager::GetInstalledKeyboardLanguages);
  module->Set(Nan::New("exports").ToLocalChecked(), newTemplate->GetFunction());
}

NODE_MODULE(keyboard_layout_manager, KeyboardLayoutManager::Init)

NAN_METHOD(KeyboardLayoutManager::New) {
  Nan::HandleScope scope;

  Local<Function> callbackHandle = info[0].As<Function>();
  Nan::Callback *callback = new Nan::Callback(callbackHandle);

  KeyboardLayoutManager *manager = new KeyboardLayoutManager(callback);
  manager->Wrap(info.This());
  return;
}

KeyboardLayoutManager::KeyboardLayoutManager(Nan::Callback *callback) : callback(callback) {
}

KeyboardLayoutManager::~KeyboardLayoutManager() {
  delete callback;
};

void KeyboardLayoutManager::HandleKeyboardLayoutChanged() {
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeyboardLayout) {
  Nan::HandleScope scope;

  char layoutName[KL_NAMELENGTH];
  if (::GetKeyboardLayoutName(layoutName))
    info.GetReturnValue().Set(Nan::New(layoutName).ToLocalChecked());
  else
    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeyboardLanguage) {
  Nan::HandleScope scope;

  HKL layout;
  DWORD dwThreadId = 0;
  HWND hWnd = GetForegroundWindow();

  if (hWnd != NULL) {
    dwThreadId = GetWindowThreadProcessId(hWnd, NULL);
  }

  layout = GetKeyboardLayout(dwThreadId);

  wchar_t buf[LOCALE_NAME_MAX_LENGTH];
  std::wstring wstr;
  LCIDToLocaleName(MAKELCID((UINT)layout & 0xFFFF, SORT_DEFAULT), buf, LOCALE_NAME_MAX_LENGTH, 0);
  wstr.assign(buf);

  std::string str = ToUTF8(wstr);
  info.GetReturnValue().Set(Nan::New<String>(str.data(), str.size()).ToLocalChecked());
}

NAN_METHOD(KeyboardLayoutManager::GetInstalledKeyboardLanguages) {
  Nan::HandleScope scope;

  int layoutCount = GetKeyboardLayoutList(0, NULL);
  HKL* layouts = new HKL[layoutCount];
  GetKeyboardLayoutList(layoutCount, layouts);

  Local<Array> result = Nan::New<Array>(layoutCount);
  wchar_t buf[LOCALE_NAME_MAX_LENGTH];

  for (int i=0; i < layoutCount; i++) {
    std::wstring wstr;
    LCIDToLocaleName(MAKELCID((UINT)layouts[i] & 0xFFFF, SORT_DEFAULT), buf, LOCALE_NAME_MAX_LENGTH, 0);
    wstr.assign(buf);

    std::string str = ToUTF8(wstr);
    result->Set(i, Nan::New<String>(str.data(), str.size()).ToLocalChecked());
  }

  delete[] layouts;
  info.GetReturnValue().Set(result);
}
