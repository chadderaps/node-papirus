#include <node.h>
#include <stdio.h>

namespace bitscreen
{

template<class T>
v8::Local<T> GetParm(v8::Isolate * isolate, v8::Local<v8::Object> obj, string str);

template<>
inline v8::Local<v8::Integer> GetParm(v8::Isolate * isolate, v8::Local<v8::Object> obj, string str)
{
  v8::Local<v8::Value> val = obj->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, str.c_str())).ToLocalChecked();
  v8::Local<v8::Integer> ret = val->ToInteger(isolate->GetCurrentContext()).ToLocalChecked();
  return ret;
}

template<>
inline v8::Local<v8::String> GetParm(v8::Isolate * isolate, v8::Local<v8::Object> obj, string str)
{
  v8::Local<v8::Value> val = obj->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, str.c_str())).ToLocalChecked();
  v8::Local<v8::String> ret = val->ToString(isolate->GetCurrentContext()).ToLocalChecked();
  return ret;
}

template<>
inline v8::Local<v8::Object> GetParm(v8::Isolate * isolate, v8::Local<v8::Object> obj, string str)
{
  v8::Local<v8::Object> ret;
  v8::Maybe<bool> has = obj->Has(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, str.c_str()));

  printf("Parent object has object %s? %s\n", str.c_str(), has.FromMaybe(false) ? "true" : "false");

  if (has.FromMaybe(false))
  {
    v8::Local<v8::Value> val = obj->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, str.c_str())).ToLocalChecked();
    ret = val->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
  }
  
  return ret;
}

};
