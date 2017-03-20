#include "bitobject.h"
#include "bitscreen.h"
#include "bitscreen_util.h"
#include "bitfonts.h"
#include "bitchar.h"
#include "utf8iterator.h"

namespace bitscreen
{
  using v8::Isolate;
  using v8::Local;
  using v8::MaybeLocal;
  using v8::Object;
  using v8::FunctionCallbackInfo;
  using v8::Value;
  using v8::String;
  using v8::FunctionTemplate;
  using v8::ObjectTemplate;
  using v8::Persistent;
  using v8::Function;
  using v8::PropertyCallbackInfo;
  using v8::Integer;
  using v8::Context;

Persistent<Function> BitObject::constructor;

void BitObject::Init(Isolate * isolate)
{
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "BitObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> objTpl = tpl->InstanceTemplate();

  objTpl->SetAccessor(String::NewFromUtf8(isolate, "x"), GetX);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "y"), GetY);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "value"), GetValue, SetValue);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "name"), GetName);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "height"), GetHeight);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "width"), GetWidth);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "align"), GetAlign);
  objTpl->SetAccessor(String::NewFromUtf8(isolate, "alignToObj"), GetAlignTo);

  constructor.Reset(isolate, tpl->GetFunction());
}

void BitObject::NewInstance(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();

  printf("New Instance\n");

  const int argc = 2;
  Local<Value> argv[argc] = {args.Holder(), args[0]};

  Local<Function> con = Local<Function>::New(isolate, constructor);
  Local<Context> context = isolate->GetCurrentContext();

  printf("Calling Constructor\n");
  Local<Object> instance = con->NewInstance(context, argc, argv).ToLocalChecked();
  printf("Returning\n");

  args.GetReturnValue().Set(instance);
}

void BitObject::New(const FunctionCallbackInfo<Value>& args)
{
  Isolate * isolate = args.GetIsolate();

  printf("New\n");

  if (args.IsConstructCall())
  {
    Local<Object> screenObj = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    Local<Object> parms = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

    BitImage * screen = ObjectWrap::Unwrap<BitImage>(screenObj);

    printf("Screen is 0x%016llX\n", (int64_t)screen);
    printf("Screen name is %s\n", screen->name.c_str());

    String::Utf8Value nameStr(GetParm<String>(isolate, parms, "name"));
    String::Utf8Value alignStr(GetParm<String>(isolate, parms, "align"));
    Local<Integer> x = GetParm<Integer>(isolate, parms, "x");
    Local<Integer> y = GetParm<Integer>(isolate, parms, "y");
    Local<Integer> size = GetParm<Integer>(isolate, parms, "size");
    Local<Object> alignObjVal = GetParm<Object>(isolate, parms, "alignToObj");

    ALIGNMENT alignVal = GetAlignment(*alignStr);

    BitObject * obj = new BitObject(screen, *nameStr, "", x->Value(), y->Value(), alignVal);

    if (not alignObjVal.IsEmpty())
    {
      BitObject * alignObj = ObjectWrap::Unwrap<BitObject>(alignObjVal);
      obj->alignTo = alignObj;
    }

    obj->fontSize = size->Value();

    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  }
  else
  {
    printf("Shouldn't Get Here\n");
    NewInstance(args);
  }
}

void BitObject::SetValue(Local<String> property, Local<Value> val, const PropertyCallbackInfo<void>& info)
{
  Isolate * isolate = info.GetIsolate();
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());

  String::Utf8Value valStr(val->ToString(isolate->GetCurrentContext()).ToLocalChecked());

  obj->value = *valStr;
}

void BitObject::GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  info.GetReturnValue().Set(obj->Width());
}
void BitObject::GetHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  info.GetReturnValue().Set(obj->Height());
}
void BitObject::GetX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  info.GetReturnValue().Set(obj->x);
}
void BitObject::GetY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  info.GetReturnValue().Set(obj->y);
}
void BitObject::GetFontSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  info.GetReturnValue().Set(obj->fontSize);
}
void BitObject::GetAlign(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  Isolate * isolate = info.GetIsolate();
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  string alignStr = GetAlignmentString(obj->align);
  Local<String> val = String::NewFromUtf8(isolate, alignStr.c_str());
  info.GetReturnValue().Set(val);
}
void BitObject::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  Isolate * isolate = info.GetIsolate();
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  Local<String> val = String::NewFromUtf8(isolate, obj->name.c_str());
  info.GetReturnValue().Set(val);
}
void BitObject::GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  Isolate * isolate = info.GetIsolate();
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());
  Local<String> val = String::NewFromUtf8(isolate, obj->value.c_str());
  info.GetReturnValue().Set(val);
}
void BitObject::GetAlignTo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
  Isolate * isolate = info.GetIsolate();
  BitObject * obj = ObjectWrap::Unwrap<BitObject>(info.Holder());

  if (obj->alignTo)
  {
    info.GetReturnValue().Set(obj->alignTo->persistent());
  }
  else
  {
    info.GetReturnValue().Set(Null(isolate));
  }
}

BitObject::BitObject(BitImage * s, string n, string v, int ex, int why, ALIGNMENT a) :
  screen(s), name(n), value(v), x(ex), y(why), align(a), alignTo(NULL)
{
  fontName = "Menlo";
}

BitObject::~BitObject() {}

BitObject::ALIGNMENT BitObject::GetAlignment(string align)
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

string BitObject::GetAlignmentString(BitObject::ALIGNMENT align)
{
  string ret("");

  if (align & ALIGNMENT_BOTTOM) ret += "BOTTOM";
  if (align & ALIGNMENT_TOP) ret += "TOP";

  if (align & ALIGNMENT_RIGHT) ret += "RIGHT";
  if (align & ALIGNMENT_LEFT) ret += "LEFT";

  printf("%s\n", ret.c_str());

  return ret;
}

int BitObject::X()
{
  if (not alignTo)
  {
    return x;
  }
  else
  {
    return x + alignTo->X() + alignTo->Width();
  }
}

int BitObject::Y()
{
  if (not alignTo)
  {
    return y;
  }
  else
  {
    return y + alignTo->Y();
  }
}

int BitObject::Height()
{
  int height = 0;

  for (auto c = utf8iterator(value); c != value.end(); ++c)
  {
    CharFont::BitCharBuffer * image = NULL;

    int err = screen->fonts.GetChar(fontName, fontSize, *c, image);

    if (err) return 0;

    if (image->Top() > height)
    {
      height = image->Top();
    }
  }

  return height;
}

int BitObject::Width()
{
  int width = 0;

  for (auto c = utf8iterator(value); c != value.end(); ++c)
  {
    CharFont::BitCharBuffer * image = NULL;

    int err = screen->fonts.GetChar(fontName, fontSize, *c, image);

    if (err) return 0;

    width += image->AdvanceRight();
  }

  return width;
}

};

extern "C" {
int z_verbose = 0;

void z_error (char* message)
{
  printf("%s", message);
}

};
