#include "keyboard-layout-observer.h"

using namespace v8;

void KeyboardLayoutObserver::Init(Handle<Object> target) {
  Nan::HandleScope scope;
  Local<FunctionTemplate> newTemplate = Nan::New<FunctionTemplate>(KeyboardLayoutObserver::New);
  newTemplate->SetClassName(Nan::New<String>("KeyboardLayoutObserver").ToLocalChecked());
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Local<ObjectTemplate> proto = newTemplate->PrototypeTemplate();
  Nan::SetMethod(proto, "getCurrentKeyboardLayout", KeyboardLayoutObserver::GetCurrentKeyboardLayout);
  Nan::SetMethod(proto, "getCurrentKeyboardLanguage", KeyboardLayoutObserver::GetCurrentKeyboardLayout); // NB:  Intentionally mapped to same stub
  Nan::SetMethod(proto, "getInstalledKeyboardLanguages", KeyboardLayoutObserver::GetInstalledKeyboardLanguages);
  target->Set(Nan::New<String>("KeyboardLayoutObserver").ToLocalChecked(), newTemplate->GetFunction());
}

NODE_MODULE(keyboard_layout_observer, KeyboardLayoutObserver::Init)

NAN_METHOD(KeyboardLayoutObserver::New) {
  Nan::HandleScope scope;

  Local<Function> callbackHandle = info[0].As<Function>();
  Nan::Callback *callback = new Nan::Callback(callbackHandle);

  KeyboardLayoutObserver *observer = new KeyboardLayoutObserver(callback);
  observer->Wrap(info.This());
  return;
}

KeyboardLayoutObserver::KeyboardLayoutObserver(Nan::Callback *callback) : callback(callback) {
}

KeyboardLayoutObserver::~KeyboardLayoutObserver() {
  delete callback;
};

void KeyboardLayoutObserver::HandleKeyboardLayoutChanged() {
}

NAN_METHOD(KeyboardLayoutObserver::GetCurrentKeyboardLayout) {
  Nan::HandleScope scope;
  return;
}

NAN_METHOD(KeyboardLayoutObserver::GetInstalledKeyboardLanguages) {
  Nan::HandleScope scope;
  return;
}
