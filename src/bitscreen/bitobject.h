#ifndef bitobject_h
#define bitobject_h

#include <string>
#include <node.h>
#include <node_object_wrap.h>

namespace bitscreen {

class BitImage;
using std::string;

class BitObject : public node::ObjectWrap
{
  typedef v8::FunctionCallbackInfo<v8::Value> FuncArgs;

  static v8::Persistent<v8::Function> constructor;

  enum ALIGNMENT
  {
    ALIGNMENT_TOP = 0x1,
    ALIGNMENT_BOTTOM = 0x2,
    ALIGNMENT_LEFT = 0x4,
    ALIGNMENT_RIGHT = 0x8,
    ALIGNMENT_TOPLEFT = ALIGNMENT_TOP | ALIGNMENT_LEFT,
    ALIGNMENT_TOPRIGHT = ALIGNMENT_TOP | ALIGNMENT_RIGHT,
    ALIGNMENT_BOTTOMLEFT = ALIGNMENT_BOTTOM | ALIGNMENT_LEFT,
    ALIGNMENT_BOTTOMRIGHT = ALIGNMENT_BOTTOM | ALIGNMENT_RIGHT,
    ALIGNMENT_DEFAULT = ALIGNMENT_TOPLEFT
  };

public:

  static void Init(v8::Isolate * isolate);
  static void NewInstance(const FuncArgs& args);

  int X();
  int Y();
  int FontSize() { return fontSize; }
  string ValueStr() { return value; }
  string Name() { return name; }
  ALIGNMENT Align() { return align; }
  int Height();
  int Width();

private:

  static void New(const FuncArgs& args);
  static void SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
  static void GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetFontSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetAlign(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetAlignTo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

  BitObject(BitImage * screen, string n, string value, int ex, int why, ALIGNMENT a);
  ~BitObject();

  static ALIGNMENT GetAlignment(string align);
  static string GetAlignmentString(ALIGNMENT align);

  BitImage * screen;
  string name;
  string value;
  int x, y;
  int fontSize;
  string fontName;
  ALIGNMENT align;
  struct {
    BitObject * obj;
    ALIGNMENT align;
  } alignTo;
};

};

#endif
