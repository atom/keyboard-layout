#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601

#undef WINVER
#define WINVER 0x0601

#include "keyboard-layout-manager.h"

#include <string>
#include <cctype>
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
  Nan::SetMethod(proto, "getCurrentKeymap", KeyboardLayoutManager::GetCurrentKeymap);

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

struct KeycodeMapEntry {
  UINT scanCode;
  const char *dom3Code;
};

#define USB_KEYMAP_DECLARATION static const KeycodeMapEntry keyCodeMap[] =
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) {win, code}

#include "keycode_converter_data.inc"

Local<Value> CharacterForNativeCode(HKL keyboardLayout, UINT keyCode, UINT scanCode,
                                     BYTE *keyboardState, bool shift, bool altGraph) {
  memset(keyboardState, 0, 256);
  if (shift) {
    keyboardState[VK_SHIFT] = 0x80;
  }

  if (altGraph) {
    keyboardState[VK_MENU] = 0x80;
    keyboardState[VK_CONTROL] = 0x80;
  }

  wchar_t characters[5];
  int count = ToUnicodeEx(keyCode, scanCode, keyboardState, characters, 5, 0, keyboardLayout);

  if (count > 0 && !std::iscntrl(characters[0])) {
    return Nan::New<String>(reinterpret_cast<const uint16_t *>(characters), count).ToLocalChecked();
  } else {
    return Nan::Null();
  }
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeymap) {
  BYTE keyboardState[256];
  HKL keyboardLayout = GetKeyboardLayout(0);

  Handle<Object> result = Nan::New<Object>();
  Local<String> unmodifiedKey = Nan::New("unmodified").ToLocalChecked();
  Local<String> withShiftKey = Nan::New("withShift").ToLocalChecked();
  Local<String> withAltGraphKey = Nan::New("withAltGraph").ToLocalChecked();
  Local<String> withAltGraphShiftKey = Nan::New("withAltGraphShift").ToLocalChecked();

  size_t keyCodeMapSize = sizeof(keyCodeMap) / sizeof(keyCodeMap[0]);
  for (size_t i = 0; i < keyCodeMapSize; i++) {
    const char *dom3Code = keyCodeMap[i].dom3Code;
    UINT scanCode = keyCodeMap[i].scanCode;

    if (dom3Code && scanCode > 0x0000) {
      UINT keyCode = MapVirtualKeyEx(scanCode, MAPVK_VSC_TO_VK, keyboardLayout);

      Local<String> dom3CodeKey = Nan::New(dom3Code).ToLocalChecked();
      Local<Value> unmodified = CharacterForNativeCode(keyboardLayout, keyCode, scanCode, keyboardState, false, false);
      Local<Value> withShift = CharacterForNativeCode(keyboardLayout, keyCode, scanCode, keyboardState, true, false);
      Local<Value> withAltGraph = CharacterForNativeCode(keyboardLayout, keyCode, scanCode, keyboardState, false, true);
      Local<Value> withAltGraphShift = CharacterForNativeCode(keyboardLayout, keyCode, scanCode, keyboardState, true, true);

      if (unmodified->IsString() || withShift->IsString() || withAltGraph->IsString() || withAltGraphShift->IsString()) {
        Local<Object> entry = Nan::New<Object>();
        entry->Set(unmodifiedKey, unmodified);
        entry->Set(withShiftKey, withShift);
        entry->Set(withAltGraphKey, withAltGraph);
        entry->Set(withAltGraphShiftKey, withAltGraphShift);

        result->Set(dom3CodeKey, entry);
      }
    }
  }

  info.GetReturnValue().Set(result);

}
