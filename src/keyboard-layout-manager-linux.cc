#include "keyboard-layout-manager.h"

using namespace v8;

void KeyboardLayoutManager::Init(Handle<Object> exports, Handle<Object> module) {
  Nan::HandleScope scope;
  Local<FunctionTemplate> newTemplate = Nan::New<FunctionTemplate>(KeyboardLayoutManager::New);
  newTemplate->SetClassName(Nan::New<String>("KeyboardLayoutManager").ToLocalChecked());
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Local<ObjectTemplate> proto = newTemplate->PrototypeTemplate();
  Nan::SetMethod(proto, "getCurrentKeyboardLayout", KeyboardLayoutManager::GetCurrentKeyboardLayout);
  Nan::SetMethod(proto, "getCurrentKeyboardLanguage", KeyboardLayoutManager::GetCurrentKeyboardLayout); // NB:  Intentionally mapped to same stub
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
  return;
}

NAN_METHOD(KeyboardLayoutManager::GetInstalledKeyboardLanguages) {
  Nan::HandleScope scope;
  return;
}

NAN_METHOD(KeyboardLayoutManager::GetCurrentKeymap) {
  Nan::ThrowError("getCurrentKeymap is not implemented on Linux");
}
