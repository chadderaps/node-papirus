#include "bitimage.h"
#include "bitimage_fonts.h"
#include <stdio.h>

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
    for (int i = 0; i < Size(); ++i)
    {
      buffer[i] = 0xFF;
    }
  }

  return buffer != NULL;
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

  return true;
}

int BitImage::SetChar(char c, int size, int x, int y)
{
  if (not buffer) return -5;
  if (x > width or y > height)
  {
    return -1;
  }

  const CharFont::char_font * image = CharFont::getChar(c, size);

  if (image == NULL) return -2;
  if (image->width + x >= width) return -3;
  if (image->height + y >= height) return -4;

  int hCount = image->height;
  int wCount = image->width / 8;
  unsigned char mask = 0x00;
  if (image->width & 0x7)
  {
    mask = 0xFF >> (image->width & 0x7);
    wCount++;
  }

  printf("Char size is %u %u\n", image->height, image->width);

  int count = hCount * wCount;
  int scrWidth = width / 8;
  int wStart = x / 8;

  for (int i = 0; i < count; i++)
  {
    unsigned char byte = image->data[i];
    buffer[wStart + i % wCount] = byte;

    if (i and (i % wCount) == 0)
    {
      buffer[wStart + i % wCount] |= mask;
      wStart += scrWidth;
    }
  }

  return 1;
}
