#include "bitimage.h"
#include "bitimage_fonts.h"
#include <stdio.h>
#include <string.h>

namespace bitimage
{

using v8::Persistent;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
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

template<class T>
Local<T> GetParm(Isolate * isolate, Local<Object> obj, string str);

template<>
Local<Integer> GetParm(Isolate * isolate, Local<Object> obj, string str)
{
  Local<Value> val = obj->Get(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, str.c_str())).ToLocalChecked();
  Local<Integer> ret = val->ToInteger(isolate->GetCurrentContext()).ToLocalChecked();
  return ret;
}

template<>
Local<String> GetParm(Isolate * isolate, Local<Object> obj, string str)
{
  Local<Value> val = obj->Get(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, str.c_str())).ToLocalChecked();
  Local<String> ret = val->ToString(isolate->GetCurrentContext()).ToLocalChecked();
  return ret;
}

void BitImage::Init(Local<Object> exports)
{
  Isolate * isolate = exports->GetIsolate();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "BitImage"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "AddObject1", AddObject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "GetObject", GetObject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "SetValue", SetValue);
  NODE_SET_PROTOTYPE_METHOD(tpl, "Draw", Draw);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "BitImage"), tpl->GetFunction());
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
    BitImage * obj = new BitImage();
    String::Utf8Value charName(name);
    obj->Init(*charName, width->Value(), height->Value());

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

  Local<Object> parms = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

  Local<String> name = GetParm<String>(isolate, parms, "name");
  Local<String> align = GetParm<String>(isolate, parms, "align");
  Local<Integer> x = GetParm<Integer>(isolate, parms, "x");
  Local<Integer> y = GetParm<Integer>(isolate, parms, "y");
  Local<Integer> size = GetParm<Integer>(isolate, parms, "size");

  String::Utf8Value charName(name);
  String::Utf8Value charAlign(align);
  ALIGNMENT alignVal = GetAlignment(*charAlign);

  string strName(*charName);

  obj->objects[strName] = bitimage_object(obj, *charName, x->Value(), y->Value(), size->Value(), alignVal);
}

void BitImage::GetObject(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  BitImage * obj = ObjectWrap::Unwrap<BitImage>(args.Holder());

  Local<Object> parms = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

  Local<String> name = GetParm<String>(isolate, parms, "name");
  String::Utf8Value charName(name);

  bitimage_object & bObj = obj->objects[*charName];

  Local<String> value = String::NewFromUtf8(isolate, bObj.value.c_str());
  Local<Integer> x = Integer::New(isolate, bObj.x);
  Local<Integer> y = Integer::New(isolate, bObj.y);
  Local<Integer> size = Integer::New(isolate, bObj.size);
  Local<String> align = String::NewFromUtf8(isolate, GetAlignmentString(bObj.align).c_str());

  Local<Object> retObj = Object::New(isolate);

  retObj->Set(context, String::NewFromUtf8(isolate, "name"), name);
  retObj->Set(context, String::NewFromUtf8(isolate, "x"), x);
  retObj->Set(context, String::NewFromUtf8(isolate, "y"), y);
  retObj->Set(context, String::NewFromUtf8(isolate, "size"), size);
  retObj->Set(context, String::NewFromUtf8(isolate, "align"), align);
  retObj->Set(context, String::NewFromUtf8(isolate, "value"), value);

  args.GetReturnValue().Set(retObj);
}

void BitImage::SetValue(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  BitImage * obj = ObjectWrap::Unwrap<BitImage>(args.Holder());

  Local<Object> parms = args[0]->ToObject(context).ToLocalChecked();

  Local<String> name = GetParm<String>(isolate, parms, "name");
  Local<String> value = GetParm<String>(isolate, parms, "value");
  String::Utf8Value charName(name);
  String::Utf8Value charValue(value);

  bitimage_object & bObj = obj->objects[*charName];

  bObj.value = *charValue;

  GetObject(args);
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
    auto & bObj = iter->second;

    printf("Drawing object %s\n", bObj.name.c_str());
    printf("  Value is %s\n", bObj.value.c_str());

    int x = bObj.x;
    int y = bObj.y;

    printf("Alignment is 0x%08X\n", bObj.align);

    if (bObj.align & ALIGNMENT_TOP)
    {
      y += bObj.Height();
    }
    else if ((bObj.align & ALIGNMENT_BOTTOM) == 0)
    {
      y += bObj.Height() / 2;
      printf("Height was set to %u\n", bObj.Height() / 2);
    }

    if (bObj.align & ALIGNMENT_RIGHT)
    {
      x -= bObj.Width();
    }
    else if ((bObj.align & ALIGNMENT_LEFT) == 0)
    {
      x -= bObj.Width() / 2;
    }

    int status = obj->SetString(bObj.value, bObj.size, x, y);

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

BitImage::ALIGNMENT BitImage::GetAlignment(string align)
{
  if (align.compare("TOP") == 0)
  {
    return ALIGNMENT_TOP;
  }
  else if (align.compare("TOPLEFT") == 0)
  {
    return ALIGNMENT(ALIGNMENT_TOP | ALIGNMENT_LEFT);
  }
  else if (align.compare("TOPRIGHT") == 0)
  {
    return ALIGNMENT(ALIGNMENT_TOP | ALIGNMENT_RIGHT);
  }
  else if (align.compare("BOTTOMRIGHT") == 0)
  {
    return ALIGNMENT(ALIGNMENT_BOTTOM | ALIGNMENT_RIGHT);
  }
  else if (align.compare("BOTTOMLEFT") == 0)
  {
    return ALIGNMENT(ALIGNMENT_BOTTOM | ALIGNMENT_LEFT);
  }
  else if (align.compare("BOTTOM") == 0)
  {
    return ALIGNMENT_BOTTOM;
  }
  else if (align.compare("RIGHT") == 0)
  {
    return ALIGNMENT_RIGHT;
  }
  else if (align.compare("LEFT") == 0)
  {
    return ALIGNMENT_LEFT;
  }

  return ALIGNMENT(ALIGNMENT_BOTTOM | ALIGNMENT_LEFT);
}

string BitImage::GetAlignmentString(BitImage::ALIGNMENT align)
{
  string ret("");

  if (align & ALIGNMENT_BOTTOM) ret += "BOTTOM";
  if (align & ALIGNMENT_TOP) ret += "TOP";

  if (align & ALIGNMENT_RIGHT) ret += "RIGHT";
  if (align & ALIGNMENT_LEFT) ret += "LEFT";

  return ret;
}

int BitImage::bitimage_object::Height()
{
  int height = 0;

  for (auto c = value.begin(); c != value.end(); ++c)
  {
    CharFont::BitCharBuffer * image = NULL;

    int err = parent->fonts.GetChar("Menlo", size, *c, image);

    if (image->Top() > height)
    {
      height = image->Top();
    }
  }

  printf("BitImage object height is %d\n", height);

  return height;
}

int BitImage::bitimage_object::Width()
{
  int width = 0;

  for (auto c = value.begin(); c != value.end(); ++c)
  {
    CharFont::BitCharBuffer * image = NULL;

    int err = parent->fonts.GetChar("Menlo", size, *c, image);

    width += image->AdvanceRight();
  }

  return width;
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

bool BitImage::Init(string n, int w, int h)
{
  name = n;
  width = w;
  height = h;

  if (not alloc()) return false;

  fonts.Init(96);

  if (fonts.Load("fonts/Menlo.ttc", "Menlo"))
  {
    printf("Couldn't load Menlow font\n");
  }

  return true;
}

int BitImage::SetString(string str, int size, int x, int y)
{
  int status = 0;

  for (auto c = str.begin(); c != str.end(); ++c)
  {
    CharFont::BitCharBuffer * image = NULL;

    status = fonts.GetChar("Menlo", size, *c, image);

    if (status != 0)
    {
      printf("Failed to get char %c\n", *c);
      return -1;
    }

    printf("Shifts are 0x%08X 0x%08X\n", image->ShiftRight(), image->Top());

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

  int hCount = image->Height();
  int wCount = (image->Width()) / 8;
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
    wCount++;
  }

  printf("Char size is %u %u\n", image->Height(), image->Width());
  printf("X Shift is %u\n", xShift);

  int count = hCount * wCount;
  int scrWidth = width / 8;
  int wStart = x / 8 + y * scrWidth;

  unsigned char maskByte = 0xFF << (8 - xShift);
  unsigned char nextByte = maskByte & buffer[wStart];

  int i = 0;

  for (auto iter = image->begin(); iter != image->end(); ++iter, ++i)
  {
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
    }
  }

  return 1;
}

};
