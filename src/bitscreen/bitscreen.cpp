#include "bitscreen.h"
#include "bitscreen_util.h"
#include "bitobject.h"
#include "bitfonts.h"
#include "utf8iterator.h"
#include <stdio.h>
#include <string.h>

namespace bitscreen
{

using v8::Persistent;
using v8::FunctionCallbackInfo;
using v8::PropertyCallbackInfo;
using v8::FunctionTemplate;
using v8::ObjectTemplate;
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

Persistent<Function> BitImage::constructor;

void BitImage::Init(Local<Object> exports)
{
  Isolate * isolate = exports->GetIsolate();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "BitImage"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "AddObject", AddObject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "GetObject", GetObject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "Draw", Draw);

  Local<ObjectTemplate> objTpl = tpl->InstanceTemplate();

  objTpl->SetAccessor(String::NewFromUtf8(isolate, "width"), GetWidth);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "height"), GetHeight);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "name"), GetName);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "dpi"), GetDPI);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "BitScreen"), tpl->GetFunction());
}

void BitImage::New(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();

  if (args.IsConstructCall())
  {
    Local<Object> parms = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    Local<String> name = GetParm<String>(isolate, parms, "name");
    Local<Integer> width = GetParm<Integer>(isolate, parms, "width");
    Local<Integer> height = GetParm<Integer>(isolate, parms, "height");
    Local<Integer> dpi = GetParm<Integer>(isolate, parms, "dpi");
    BitImage * obj = new BitImage();
    String::Utf8Value charName(name);
    obj->Init(*charName, width->Value(), height->Value(), dpi->Value());

    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  }
  else
  {
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Object> result = cons->NewInstance(context, argc, argv).ToLocalChecked();
    args.GetReturnValue().Set(result);
  }
}

void BitImage::AddObject(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();
  BitImage * obj = ObjectWrap::Unwrap<BitImage>(args.Holder());

  BitObject::NewInstance(args);

  Local<Object> bObjVal = args.GetReturnValue().Get()->ToObject(isolate);
  BitObject * bObj = ObjectWrap::Unwrap<BitObject>(bObjVal);

  obj->objects[bObj->Name()] = bObj;
}

void BitImage::GetObject(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();

  BitImage * obj = ObjectWrap::Unwrap<BitImage>(args.Holder());

  Local<Object> parms = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

  Local<String> name = GetParm<String>(isolate, parms, "name");
  String::Utf8Value charName(name);

  BitObject * bObj = obj->objects[*charName];

  args.GetReturnValue().Set(bObj->persistent());
}

void BitImage::Draw(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();
  const int argc = 2;

  BitImage * obj = ObjectWrap::Unwrap<BitImage>(args.Holder());

  Local<Function> cb = Local<Function>::Cast(args[0]);

  obj->ResetBuffer();

  for (auto iter = obj->objects.begin(); iter != obj->objects.end(); ++iter)
  {
    auto bObj = iter->second;

    printf("Drawing object %s\n", bObj->Name().c_str());
    printf("  Value is %s\n", bObj->ValueStr().c_str());

    int x = bObj->X();
    int y = bObj->Y();

    if (bObj->Align() & ALIGNMENT_TOP)
    {
      y += bObj->Height();
    }
    else if ((bObj->Align() & ALIGNMENT_BOTTOM) == 0)
    {
      y += bObj->Height() / 2;
      printf("Height was set to %u\n", bObj->Height() / 2);
    }

    if (bObj->Align() & ALIGNMENT_RIGHT)
    {
      x -= bObj->Width();
    }
    else if ((bObj->Align() & ALIGNMENT_LEFT) == 0)
    {
      x -= bObj->Width() / 2;
    }

    printf("  Start is %d, %d\n", x, y);

    int status = obj->SetString(bObj->ValueStr(), bObj->FontSize(), x, y);

    if (status < 0)
    {
      string err = "Error when writeing string ";
      err += to_string(status);
      Local<Value> argv[argc] = {
        Exception::Error(String::NewFromUtf8(isolate, err.c_str())),
        Null(isolate)
      };

      cb->Call(Null(isolate), argc, argv);
      return;
    }
  }

  Local<Value> out = obj->Buffer(isolate);

  if (out->IsNativeError())
  {
    Local<Value> argv[argc] = {
      out,
      Null(isolate)
    };
    cb->Call(Null(isolate), argc, argv);
    return;
  }

  Local<Value> argv[argc] = {
    Null(isolate),
    out
  };

  cb->Call(Null(isolate), argc, argv);

  args.GetReturnValue().Set(Null(isolate));
}

Local<Value> BitImage::Buffer(Isolate * isolate)
{
  v8::MaybeLocal<Object> maybe_out = node::Buffer::New(isolate, Size());

  Local<Object> out;

  if (not maybe_out.ToLocal(&out))
  {
    return Exception::TypeError(
      String::NewFromUtf8(isolate, "Could not allocate new buffer")
    );
  }

  unsigned char * outData = (unsigned char *)node::Buffer::Data(out);

  memcpy(outData, buffer, Size());

  return out;
}

void BitImage::GetWidth(Local<String> property, const PropertyCallbackInfo<Value>& info)
{
  BitImage * obj = ObjectWrap::Unwrap<BitImage>(info.Holder());
  info.GetReturnValue().Set(obj->width);
}
void BitImage::GetHeight(Local<String> property, const PropertyCallbackInfo<Value>& info)
{
  BitImage * obj = ObjectWrap::Unwrap<BitImage>(info.Holder());
  info.GetReturnValue().Set(obj->height);
}
void BitImage::GetDPI(Local<String> property, const PropertyCallbackInfo<Value>& info)
{
  BitImage * obj = ObjectWrap::Unwrap<BitImage>(info.Holder());
  info.GetReturnValue().Set(obj->dpi);
}
void BitImage::GetName(Local<String> property, const PropertyCallbackInfo<Value>& info)
{
  BitImage * obj = ObjectWrap::Unwrap<BitImage>(info.Holder());
  Local<String> str = String::NewFromUtf8(info.GetIsolate(), obj->name.c_str());
  info.GetReturnValue().Set(str);
}

bool BitImage::alloc()
{
  if (buffer) delete buffer;

  buffer = NULL;

  if (width != 0 or height != 0)
  {
    buffer = new unsigned char[Size()];
  }

  if (buffer)
  {
    ResetBuffer();
  }

  return buffer != NULL;
}

void BitImage::ResetBuffer()
{
  memset(buffer, 0xFF, Size());
}

BitImage::BitImage() :
  width(0), height(0), buffer(NULL)
{
}

BitImage::~BitImage()
{
  if (buffer) delete buffer;
}

bool BitImage::Init(string n, int w, int h, int d)
{
  width = w;
  height = h;
  dpi = d;
  name = n;

  if (not alloc()) return false;

  fonts.Init(dpi);

  if (fonts.Load("fonts/Menlo.ttc", "Menlo"))
  {
    printf("Couldn't load Menlow font\n");
  }

  return true;
}

int BitImage::SetString(string str, int size, int x, int y)
{
  int status = 0;

  for (auto c = utf8iterator(str); c != str.end(); ++c)
  {
    CharFont::BitCharBuffer * image = NULL;

    status = fonts.GetChar("Menlo", size, *c, image);

    if (status != 0)
    {
      printf("Failed to get char %c\n", *c);
      return -1;
    }

    printf("Shifts are 0x%08X 0x%08X\n", image->ShiftRight(), image->Top());
    printf("Pitch is %d\n", image->Pitch());

    printf("Writing char %c at %u %u\n", *c, x + image->ShiftRight(), y - image->Top());

    status = AddChar(image, x + image->ShiftRight(), y - image->Top());

    if (status < 0)
    {
      printf("Failed on char %c\n", *c);
      break;
    }

    x += image->AdvanceRight();
  }

  return status;
}

int BitImage::SetChar(char c, int size, int x, int y)
{
  if (not buffer) return -5;
  if (x > width or y > height)
  {
    return -1;
  }

  printf("X, Y = %u, %u\n", x, y);

  CharFont::BitCharBuffer * image = NULL;

  int status = fonts.GetChar("Menlo", size, c, image);

  if (status != 0)
  {
    printf("Failed to get char %c\n", c);
    return status;
  }

  return AddChar(image, x + image->ShiftRight(), y);
}

int BitImage::AddChar(CharFont::BitCharBuffer * image, int x, int y)
{

  if (image == NULL) return -2;
  if (image->Width() + x >= width)
  {
    printf("Image width is %u\n", image->Width());
    return -3;
  }
  if (image->Height() + y >= height) return -4;

  printf("X, Y = %u, %u\n", x, y);

  int xShift = x & 0x7;

  unsigned char mask = 0x00;
  bool addFinalByte = false;

  if (image->Width() & 0x7)
  {
    int maskShift = ((image->Width() & 0x7) + xShift);
    if (maskShift > 8)
    {
      maskShift -= 8;
      if (maskShift) addFinalByte = true;
    }
    if (maskShift) mask = 0xFF >> maskShift;
  }

  printf("Char size is %u %u\n", image->Height(), image->Width());
  printf("X Shift is %u\n", xShift);

  int scrWidth = width / 8;
  int wStart = x / 8 + y * scrWidth;

  unsigned char maskByte = 0xFF << (8 - xShift);
  unsigned char nextByte = 0;

  //int i = 0;
  int offset = 0;

  for (auto iter = image->begin(); iter != image->end(); ++iter)
  {
    unsigned char byte = *iter;
    offset = iter.Row() * scrWidth + iter.Col();

    if (maskByte && iter.Col() == 0)
    {
      nextByte = buffer[wStart + offset] & maskByte;
    }

    unsigned char tempByte = byte >> xShift;
    //printf("Updated %u with 0x%02X | 0x%02X\n", wStart + i % wCount, tempByte, nextByte);
    buffer[wStart + offset] = tempByte | nextByte;
    nextByte = byte << (8 - xShift);

    if (iter.LastCol() and addFinalByte)
    {
      buffer[wStart + offset + 1] = nextByte | mask;
    }
    /*
    unsigned char byte = *iter;
    //printf("Byte is 0x%02X\n", byte);

    if (xShift == 0)
    {
      buffer[wStart + i % wCount] = byte;
      //printf("Updated %u with 0x%02X\n", wStart + i % wCount, byte);
    }
    else
    {
      unsigned char tempByte = byte >> xShift;
      //printf("Updated %u with 0x%02X | 0x%02X\n", wStart + i % wCount, tempByte, nextByte);
      buffer[wStart + i % wCount] = tempByte | nextByte;
      nextByte = byte << (8 - xShift);
    }

    if ((i % wCount) == wCount-1)
    {
      if (addFinalByte)
      {
          buffer[wStart + 1 + i % wCount] = nextByte | mask;
      }
      else
      {
        buffer[wStart + i % wCount] |= mask;
      }
      wStart += scrWidth;
      nextByte = maskByte & buffer[wStart];
    }*/
  }

  return 1;
}

};
