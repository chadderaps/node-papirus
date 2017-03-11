#include <node.h>
#include <node_buffer.h>

namespace demo {
  using v8::FunctionCallbackInfo;
  using v8::Isolate;
  using v8::Local;
  using v8::Object;
  using v8::String;
  using v8::Function;
  using v8::Number;
  using v8::Value;
  using v8::Exception;
  using v8::Null;

  void Method(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
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
  }

  NODE_MODULE(bitimage, init)
}
