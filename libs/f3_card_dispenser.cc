#include <node.h>
#include "F3API.h"
#include <iostream>


namespace f3_dispenser {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Number;
READERHANDLE handle;
LPREADERHANDLE lp = &handle;


void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();


  args.GetReturnValue().Set(String::NewFromUtf8(
      isolate, "hello world", NewStringType::kNormal).ToLocalChecked());
}

void Connect(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  int result = F3_Connect(3,9600,0, lp);

  Local<Number> num = Number::New(isolate, result);

  args.GetReturnValue().Set(num);
}

void Disconnec(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  int result = F3_Disconnect(handle);

  Local<Number> num = Number::New(isolate, result);

  args.GetReturnValue().Set(num);
}

void Move(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  int result = F3_MoveCard(handle, MM_EJECT_TO_FRONT);

  Local<Number> num = Number::New(isolate, result);

  args.GetReturnValue().Set(num);
}

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "hello", Method);
  NODE_SET_METHOD(exports, "connect", Connect);
  NODE_SET_METHOD(exports, "disconnect", Disconnec);
  NODE_SET_METHOD(exports, "move", Move);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace demo