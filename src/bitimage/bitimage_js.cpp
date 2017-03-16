#include <node.h>
#include <node_buffer.h>
#include "bitimage_fonts.h"
#include "bitimage.h"
#include <string.h>

namespace demo {
  using v8::FunctionCallbackInfo;
  using v8::Isolate;
  using v8::Local;
  using v8::MaybeLocal;
  using v8::Object;
  using v8::String;
  using v8::Function;
  using v8::Integer;
  using v8::Value;
  using v8::Exception;
  using v8::Null;
  using v8::Context;

  void Method(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
  }

  void AddChar(const FunctionCallbackInfo<Value>& args)
  {
    Isolate* isolate = args.GetIsolate();
    const int argc = 1;

    if (not args[3]->IsFunction())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Last argument must be a callback")
      ));
      return;
    }

    Local<Function> cb = Local<Function>::Cast(args[3]);

    if (not node::Buffer::HasInstance(args[0]))
    {
      Local<Value> argv[argc] = { Exception::TypeError(
        String::NewFromUtf8(isolate, "First argument must be a buffer")
      )};

      cb->Call(Null(isolate), argc, argv);
      return;
    }

    if (not args[1]->IsString() or not args[2]->IsObject())
    {
      Local<Value> argv[argc] = { Exception::TypeError(
        String::NewFromUtf8(isolate, "Invalid Arguments")
      )};

      cb->Call(Null(isolate), argc, argv);
      return;
    }

    Local<String> str = args[1]->ToString(isolate->GetCurrentContext()).ToLocalChecked();
    Local<Object> parms = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    Local<Value> valSize = parms->Get(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "size")).ToLocalChecked();
    Local<Value> xVal = parms->Get(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "x")).ToLocalChecked();
    Local<Value> yVal = parms->Get(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "y")).ToLocalChecked();
    Local<Integer> size = valSize->ToInteger(isolate->GetCurrentContext()).ToLocalChecked();
    Local<Integer> x = xVal->ToInteger(isolate->GetCurrentContext()).ToLocalChecked();
    Local<Integer> y = yVal->ToInteger(isolate->GetCurrentContext()).ToLocalChecked();

    String::Utf8Value charStr(str);

    BitImage * screen = new BitImage();

    screen->Init("MyImage", 200, 96);
    int status = screen->SetChar((*charStr)[0], size->Value(), x->Value(), y->Value());

    if (status < 0)
    {
      string retStr("Could not allocate screen buffer ");
      retStr += to_string(status);
      Local<Value> argv[argc] = { Exception::TypeError(
        String::NewFromUtf8(isolate, retStr.c_str())
      )};

      delete screen;

      cb->Call(Null(isolate), argc, argv);
      return;
    }

    v8::MaybeLocal<Object> maybe_out = node::Buffer::New(isolate, screen->Size());

    Local<Object> out;

    if (not maybe_out.ToLocal(&out))
    {
      Local<Value> argv[argc] = { Exception::TypeError(
        String::NewFromUtf8(isolate, "Could not allocate new buffer")
      )};

      delete screen;

      cb->Call(Null(isolate), argc, argv);
      return;
    }

    unsigned char * outData = (unsigned char *)node::Buffer::Data(out);

    memcpy(outData, screen->Buffer(), screen->Size());

    delete screen;

    printf("First byte is 0x%02X\n", outData[0]);

    Local<Value> argv[2] = { Null(isolate), out };

    cb->Call(Null(isolate), 2, argv);

    return;
  }

  void Convert(const FunctionCallbackInfo<Value>& args) {
    Isolate * isolate = args.GetIsolate();
    const unsigned int argc = 2;

    if (args.Length() != 2)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong Number of arguments")
      ));
      return;
    }

    if (not args[1]->IsFunction())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Last argument must be a callback")
      ));
      return;
    }

    Local<Function> cb = Local<Function>::Cast(args[1]);

    if (not node::Buffer::HasInstance(args[0]))
    {
      Local<Value> argv[argc] = { Exception::TypeError(
        String::NewFromUtf8(isolate, "First argument must be a buffer")
      ), Null(isolate)};

      cb->Call(Null(isolate), argc, argv);
      return;
    }

    unsigned char * data = (unsigned char *)node::Buffer::Data(args[0]);
    unsigned int length = node::Buffer::Length(args[0]);
    unsigned int new_length = length / 4 / 8;
    v8::MaybeLocal<Object> maybe_out = node::Buffer::New(isolate, new_length);

    Local<Object> out;

    if (not maybe_out.ToLocal(&out))
    {
      Local<Value> argv[argc] = { Exception::TypeError(
        String::NewFromUtf8(isolate, "Could not allocate new buffer")
      ), Null(isolate)};

      cb->Call(Null(isolate), argc, argv);
      return;
    }

    unsigned char * outData = (unsigned char *)node::Buffer::Data(out);

    unsigned char byte = 0;
    unsigned char bit = 0x80;

    for (unsigned int i = 0; i < length; i += 4)
    {
      if (data[i] > 250)
      {
        byte |= bit;
      }

      bit >>= 1;

      if (bit == 0)
      {
        *outData = byte;
        bit = 0x80;
        byte = 0;
        ++outData;
      }
    }

    Local<Value> argv[argc] = {Null(isolate), out};

    cb->Call(Null(isolate), argc, argv);
  }

  void init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "hello", Method);
    NODE_SET_METHOD(exports, "convert", Convert);
    NODE_SET_METHOD(exports, "addchar", AddChar);
  }

  NODE_MODULE(bitimage, init)
}
