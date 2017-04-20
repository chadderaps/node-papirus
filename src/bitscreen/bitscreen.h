#ifndef bitimage_h
#define bitimage_h

#include <string>
#include <map>
#include "bitfonts.h"
#include "bitobject.h"
#include <node.h>
#include <node_object_wrap.h>
#include <node_buffer.h>

using namespace std;

namespace bitscreen
{

class BitImage : public node::ObjectWrap
{
public:
  static void Init(v8::Local<v8::Object> exports);

//private:

  enum ALIGNMENT
  {
    ALIGNMENT_TOP = 0x1,
    ALIGNMENT_BOTTOM = 0x2,
    ALIGNMENT_LEFT = 0x4,
    ALIGNMENT_RIGHT = 0x8
  };

  CharFont::Fonts fonts;

  int width;
  int height;
  int dpi;
  string name;
  unsigned char * buffer;
  map<string, BitObject*> objects;

  BitImage();

  ~BitImage();

  bool alloc();
  void ResetBuffer();

  int AddChar(CharFont::BitCharBuffer * image, int x, int y);

  bool Init(string n, int w, int h, int d);

  int SetChar(char c, int size, int x, int y);
  int SetString(string str, int size, int x, int y);

  //const unsigned char * Buffer() { return buffer; }
  v8::Local<v8::Value> Buffer(v8::Isolate * isolate);

  int Size() { return (width * height) / 8; }

  static ALIGNMENT GetAlignment(string align);
  static string GetAlignmentString(ALIGNMENT align);

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  // Class Prototype Functions
  static void AddObject(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetObject(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetValue(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Draw(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetDPI(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
};

};
#endif
