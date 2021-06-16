#include "keyboard-layout-manager.h"

using namespace v8;

NAN_MODULE_INIT(init) {
  Nan::HandleScope scope;
  Local<FunctionTemplate> newTemplate = Nan::New<FunctionTemplate>(KeyboardLayoutManager::New);
  newTemplate->SetClassName(Nan::New<String>("KeyboardLayoutManager").ToLocalChecked());
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> proto = newTemplate->PrototypeTemplate();

  Nan::SetMethod(proto, "getCurrentKeyboardLayout", KeyboardLayoutManager::GetCurrentKeyboardLayout);
  Nan::SetMethod(proto, "getCurrentKeyboardLanguage", KeyboardLayoutManager::GetCurrentKeyboardLanguage);
  Nan::SetMethod(proto, "getInstalledKeyboardLanguages", KeyboardLayoutManager::GetInstalledKeyboardLanguages);
  Nan::SetMethod(proto, "getCurrentKeymap", KeyboardLayoutManager::GetCurrentKeymap);

  Nan::Set(target, Nan::New("KeyboardLayoutManager").ToLocalChecked(), Nan::GetFunction(newTemplate).ToLocalChecked());
}

#if NODE_MAJOR_VERSION >= 10
NAN_MODULE_WORKER_ENABLED(keyboard_layout_manager, init)
#else
NODE_MODULE(keyboard_layout_manager, init)
#endif

NAN_METHOD(KeyboardLayoutManager::New) {
  Nan::HandleScope scope;
  Local<Function> callbackHandle = info[0].As<Function>();
  Nan::Callback *callback = new Nan::Callback(callbackHandle);

  KeyboardLayoutManager *manager = new KeyboardLayoutManager(info.GetIsolate(), callback);
  manager->Wrap(info.This());
  return;
}
