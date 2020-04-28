#include <node.h>
#include "F3API.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>

namespace f3_dispenser
{

using v8::Context;
using v8::FunctionCallbackInfo;
using v8::Integer;
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
  int result;
  Local<Number> num;
  switch (move)
  {
  case 1:
    result = F3_MoveCard(handle, MM_RETURN_TO_FRONT);
    num = Number::New(isolate, result);
    args.GetReturnValue().Set(result);
    return;
  case 2:
    result = F3_MoveCard(handle, MM_RETURN_TO_IC_POS);
    num = Number::New(isolate, result);
    args.GetReturnValue().Set(result);
    return;
  case 3:
    result = F3_MoveCard(handle, MM_RETURN_TO_RF_POS);
    num = Number::New(isolate, result);
    args.GetReturnValue().Set(result);
    return;
  case 4:
    result = F3_MoveCard(handle, MM_CAPTURE_TO_BOX);
    num = Number::New(isolate, result);
    args.GetReturnValue().Set(result);
    return;
  case 5:
    result = F3_MoveCard(handle, MM_EJECT_TO_FRONT);
    num = Number::New(isolate, result);
    args.GetReturnValue().Set(result);
    return;
  default:
    result = 1997;
    return;
  }
}

void SensorStatus(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  BYTE bStatus[NUM_SENSORS];
  int result = F3_GetSenserDetail(handle, bStatus);
  Local<Object> obj = Object::New(isolate);
  Local<Number> num = Number::New(isolate, result);
  Local<String> text;
  if (result == 0)
  {
    for (int i = 0; i < NUM_SENSORS; i++)
    {
      std::string str = std::to_string(i + 1);
      Local<Number> status;
      if (bStatus[i] == 49)
      {
        status = Number::New(isolate, 1);
      }
      else
      {
        status = Number::New(isolate, 0);
      }
      Local<String> id = String::NewFromUtf8(isolate, "S", NewStringType::kNormal).ToLocalChecked();
      Local<String> x = String::NewFromUtf8(isolate, str.c_str(), NewStringType::kNormal).ToLocalChecked();
      Local<String> concatened = String::Concat(isolate, id, x);
      obj->Set(context, concatened, status).FromJust();
    }
  }
  else
  {
    text = String::NewFromUtf8(isolate, "Read Error", NewStringType::kNormal).ToLocalChecked();
    obj->Set(context, String::NewFromUtf8(isolate, "error", NewStringType::kNormal).ToLocalChecked(), text).FromJust();
    obj->Set(context, String::NewFromUtf8(isolate, "result", NewStringType::kNormal).ToLocalChecked(), num).FromJust();
  }

  args.GetReturnValue().Set(obj);
}

void GetDispenserStatus(const FunctionCallbackInfo<Value> &args)
{
  CRSTATUS crs;
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = Object::New(isolate);
  Local<Number> laneStatus;
  Local<Number> cardBoxStatus;
  Local<Number> fullCaptureBox;

  int lResult = F3_GetCRStatus(handle, &crs);

  if (lResult == 0)
  {
    switch (crs.bLaneStatus)
    {
    case LS_NO_CARD_IN:
      laneStatus= Number::New(isolate, LS_NO_CARD_IN);
      obj->Set(context, String::NewFromUtf8(isolate, "laneStatus", NewStringType::kNormal).ToLocalChecked(), laneStatus).FromJust();
      ;
      break;
    case LS_CARD_AT_GATE_POS:
      laneStatus = Number::New(isolate, LS_CARD_AT_GATE_POS);
      obj->Set(context, String::NewFromUtf8(isolate, "laneStatus", NewStringType::kNormal).ToLocalChecked(), laneStatus).FromJust();
      break;
    case LS_CARD_IN:
      laneStatus = Number::New(isolate, LS_CARD_IN);
      obj->Set(context, String::NewFromUtf8(isolate, "laneStatus", NewStringType::kNormal).ToLocalChecked(), laneStatus).FromJust();
      break;
    }

    switch (crs.bCardBoxStatus)
    {
    case CBS_EMPTY:
      cardBoxStatus = Number::New(isolate, CBS_EMPTY);
      obj->Set(context, String::NewFromUtf8(isolate, "cardBoxStatus", NewStringType::kNormal).ToLocalChecked(), cardBoxStatus).FromJust();
      break;
    case CBS_INSUFFICIENT:
      cardBoxStatus = Number::New(isolate, CBS_INSUFFICIENT);
      obj->Set(context, String::NewFromUtf8(isolate, "cardBoxStatus", NewStringType::kNormal).ToLocalChecked(), cardBoxStatus).FromJust();
      break;
    case CBS_ENOUGH:
      cardBoxStatus = Number::New(isolate, CBS_ENOUGH);
      obj->Set(context, String::NewFromUtf8(isolate, "cardBoxStatus", NewStringType::kNormal).ToLocalChecked(), cardBoxStatus).FromJust();
      break;
    }

    if (crs.fCaptureBoxFull){
      fullCaptureBox = Number::New(isolate, 1);
      obj->Set(context, String::NewFromUtf8(isolate, "fullCaptureBox", NewStringType::kNormal).ToLocalChecked(), fullCaptureBox).FromJust();
    }
    else{
      fullCaptureBox = Number::New(isolate, 0);
      obj->Set(context, String::NewFromUtf8(isolate, "fullCaptureBox", NewStringType::kNormal).ToLocalChecked(), fullCaptureBox).FromJust();
    }
    args.GetReturnValue().Set(obj);
  }
  else
  {
    args.GetReturnValue().Set(lResult);
  }
}

void Initialize(Local<Object> exports)
{
  NODE_SET_METHOD(exports, "connect", Connect);
  NODE_SET_METHOD(exports, "disconnect", Disconnec);
  NODE_SET_METHOD(exports, "move", Move);
  NODE_SET_METHOD(exports, "sensorStatus", SensorStatus);
  NODE_SET_METHOD(exports, "checkDispenserStatus", GetDispenserStatus);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);

} // namespace f3_dispenser