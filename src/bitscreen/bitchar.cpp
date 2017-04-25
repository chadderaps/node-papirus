#include "bitchar.h"

using namespace CharFont;

BitCharBuffer::BitCharBuffer(FT_GlyphSlot glyph) :
  width(glyph->bitmap.width),
  height(glyph->bitmap.rows),
  num_bytes(glyph->bitmap.pitch * height),
  base_height(glyph->linearVertAdvance / 65536),
  pitch(glyph->bitmap.pitch),
  endIter(this, num_bytes)
{
  advance.x = glyph->advance.x / 64;
  advance.y = glyph->advance.y / 64;
  offset.left = glyph->bitmap_left;
  offset.top = glyph->bitmap_top;

  printf("glyph top is %d\n", glyph->bitmap_top);
  printf("My top is %d\n", offset.top);

  bits = new unsigned char[num_bytes+1];

  for (int i = 0; i < num_bytes; ++i)
  {
    bits[i] = glyph->bitmap.buffer[i] ^ 0xFF;
  }
}

BitChar::BitChar(FT_GlyphSlot glyph) :
  height(glyph->bitmap.rows),
  pitch(glyph->bitmap.pitch)
{
  size = pitch * height;
}

BitChar::~BitChar()
{
  for (auto iter = chars.begin(); iter != chars.end(); ++iter)
  {
    if (iter->second != NULL) delete iter->second;
  }
}

void BitChar::AddChar(FT_GlyphSlot glyph, uint32_t c)
{
  chars[c] = new BitCharBuffer(glyph);
}

BitCharBuffer * BitChar::GetChar(uint32_t c)
{
  if (chars.count(c)) return chars[c];
  else return NULL;
}

BitCharBuffer::iterator::iterator(BitCharBuffer * bcb, int offset) :
  obj(bcb), idx(offset)
{
  if (idx < obj->num_bytes) CalcCoordinates();
}

BitCharBuffer::iterator & BitCharBuffer::iterator::operator++()
{
  if (idx < obj->num_bytes) idx++;

  if (idx < obj->num_bytes) CalcCoordinates();

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
