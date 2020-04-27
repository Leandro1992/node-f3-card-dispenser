#include <node.h>
#include "F3API.h"
#include <iostream>
#include <windows.h>

namespace f3_dispenser
{

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;
READERHANDLE handle;
LPREADERHANDLE lp = &handle;

void Connect(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();
  int port = args[0].As<Number>()->Value();
  int baudrate = args[1].As<Number>()->Value();
  int result = F3_Connect(port, baudrate, 0, lp);
  Local<Number> num = Number::New(isolate, result);
  args.GetReturnValue().Set(num);
}

void Disconnec(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();
  int result = F3_Disconnect(handle);
  Local<Number> num = Number::New(isolate, result);
  args.GetReturnValue().Set(num);
}

void Move(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();
  int move = args[0].As<Number>()->Value();
  int result = -1;
  switch (move)
  {
  case 1:
    result = F3_MoveCard(handle, MM_RETURN_TO_FRONT);
    ;
  case 2:
    result = F3_MoveCard(handle, MM_RETURN_TO_IC_POS);
    ;
  case 3:
    result = F3_MoveCard(handle, MM_RETURN_TO_RF_POS);
    ;
  case 4:
    result = F3_MoveCard(handle, MM_CAPTURE_TO_BOX);
    ;
  case 5:
    result = F3_MoveCard(handle, MM_EJECT_TO_FRONT);
    ;
  default:
    result = 19971;
    ;
  }
  Local<Number> num = Number::New(isolate, result);
  args.GetReturnValue().Set(num);
} 

void Initialize(Local<Object> exports)
{
  NODE_SET_METHOD(exports, "connect", Connect);
  NODE_SET_METHOD(exports, "disconnect", Disconnec);
  NODE_SET_METHOD(exports, "move", Move);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

} // namespace f3_dispenser