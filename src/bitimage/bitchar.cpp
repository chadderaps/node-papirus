#include "bitchar.h"

using namespace CharFont;

BitCharBuffer::BitCharBuffer(FT_GlyphSlot glyph) :
  width(glyph->bitmap.width),
  height(glyph->bitmap.rows),
  num_bytes(glyph->bitmap.pitch * height),
  pitch(glyph->bitmap.pitch),
  endIter(this, num_bytes)
{
  advance.x = glyph.advance.x;
  advance.y = glyph.advance.y;
  offset.left = glyph.bitmap_left;
  offset.top = -glyph.bitmap_top;

  bits = new unsigned char[num_bytes+1];

  for (int i = 0; i < num_bytes; ++i)
  {
    bits[i] = glyph->bitmap.buffer[i] ^ 0xFF;
  }
}

BitChar::BitChar(FT_GlyphSlot glyph) :
  width(glyph->bitmap.width),
  height(glyph->bitmap.rows),
  pitch(glyph->bitmap.pitch),
  chars(256, NULL)
{
  size = pitch * height;
}

BitChar::~BitChar()
{
  for (auto iter = chars.begin(); iter != chars.end(); ++iter)
  {
    if (*iter != NULL) delete *iter;
  }
}

void BitChar::AddChar(FT_GlyphSlot glyph, char c)
{
  FT_Bitmap * bitmap = &glyph->bitmap;

  chars[c] = new BitCharBuffer(glyph);
}

BitCharBuffer * BitChar::GetChar(char c)
{
  return chars[c];
}

BitCharBuffer::iterator::iterator(BitCharBuffer * bcb, int offset) :
  obj(bcb), idx(offset)
{
  printf("0x%08X\n", (unsigned long)obj);
  CalcCoordinates();
}

BitCharBuffer::iterator & BitCharBuffer::iterator::operator++()
{
  if (idx < obj->num_bytes) idx++;

  if (idx < obj->num_bytes)
  {
    CalcCoordinates();
  }

  return *this;
}

unsigned char & BitCharBuffer::iterator::operator*()
{
  return obj->bits[idx];
}

bool BitCharBuffer::iterator::operator!=(const BitCharBuffer::iterator & other)
{
  if (obj != other.obj) return true;
  if (idx != other.idx) return true;

  return false;
}
