#ifndef bitchar_h
#define bitchar_h

#include <string>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

namespace CharFont
{

class BitCharBuffer
{
private:
  int width;
  int height;
  int num_bytes;
  int pitch;
  unsigned char * bits;

public:
  class iterator {
  private:
    BitCharBuffer * obj;
    int idx;
    int row;
    int col;

    void CalcCoordinates()
    {
      row = idx / obj->pitch;
      col = idx % obj->pitch;
    }

  public:

    iterator(BitCharBuffer * bcb, int offset);

    iterator & operator++();
    unsigned char & operator*();
    bool operator!=(const iterator & other);

    int Row() { return row; }
    int Col() { return col; }
  };

  friend class BitCharBuffer::iterator;

  typedef const iterator const_iterator;

  iterator begin() { return iterator(this, 0); }
  const iterator & end() { return endIter; }

  BitCharBuffer(int w, int h, int p, unsigned char * buffer);

  int Height() { return height; }
  int Width() { return width; }

  private:
    iterator endIter;
};

class BitChar {

private:
  int width;
  int height;
  int pitch;
  int size;
  vector<BitCharBuffer*> chars;

public:

  BitChar() {}
  BitChar(FT_GlyphSlot glyph);
  ~BitChar();

  void AddChar(FT_GlyphSlot glyph, char c);
  BitCharBuffer * GetChar(char c);
};

};
#endif /* ifndef bitchar_h */
