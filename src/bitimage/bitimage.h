#ifndef bitimage_h
#define bitimage_h

#include <string>
#include <map>
#include "bitimage_fonts.h"
#include <node.h>
#include <node_object_wrap.h>
#include <node_buffer.h>

using namespace std;

namespace bitimage
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

  struct bitimage_object
  {
    string name;
    int x, y, size;
    ALIGNMENT align;
    string value;

    bitimage_object(string n, int ex, int why, int s, ALIGNMENT a) :
      name(n), x(ex), y(why), size(s), align(a), value("")
    {}

    bitimage_object() :
      name(""), x(0), y(0), size(16), align(ALIGNMENT_TOP), value("")
    {}

    int Height();
    int Width();
  };

  int width;
  int height;
  string name;
  unsigned char * buffer;
  map<string, bitimage_object> objects;

  BitImage();

  ~BitImage();

  bool alloc();
  void ResetBuffer();

  int AddChar(const CharFont::char_font * image, int x, int y);

  bool Init(string n, int w, int h);

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
};

};
#endif
