#import <Carbon/Carbon.h>
#import "keyboard-layout-observer.h"

using namespace v8;

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

uv_loop_t *loop = uv_default_loop();
uv_async_t async;

static void notificationHandler(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
  async.data = observer;
  uv_async_send(&async);
}

static void asyncSendHandler(uv_async_t *handle) {
  (static_cast<KeyboardLayoutObserver *>(handle->data))->HandleKeyboardLayoutChanged();
}

KeyboardLayoutObserver::KeyboardLayoutObserver(NanCallback *callback) : callback(callback) {
  uv_async_init(loop, &async, asyncSendHandler);

  CFNotificationCenterAddObserver(
      CFNotificationCenterGetDistributedCenter(),
      this,
      &notificationHandler,
      kTISNotifySelectedKeyboardInputSourceChanged,
      NULL,
      CFNotificationSuspensionBehaviorDeliverImmediately
  );
}

KeyboardLayoutObserver::~KeyboardLayoutObserver() {
  delete callback;
};

void KeyboardLayoutObserver::HandleKeyboardLayoutChanged() {
  callback->Call(0, NULL);
}

NAN_METHOD(KeyboardLayoutObserver::GetCurrentKeyboardLayout) {
  NanScope();
  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFStringRef sourceId = (CFStringRef) TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
  NanReturnValue(NanNew<String>([(NSString *)sourceId UTF8String]));
}
