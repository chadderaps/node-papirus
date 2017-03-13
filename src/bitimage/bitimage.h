#ifndef bitimage_h
#define bitimage_h

#include <string>
#include "bitimage_fonts.h"

using namespace std;

class BitImage
{
private:

  int width;
  int height;
  string name;
  unsigned char * buffer;

  bool alloc();

public:

  BitImage();

  ~BitImage();

  bool Init(string n, int w, int h);

  int SetChar(char c, int size, int x, int y);

  const unsigned char * Buffer() { return buffer; }

  int Size() { return width * height; }
};

#endif
