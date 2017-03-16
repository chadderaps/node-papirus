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

int BitImage::SetString(string str, int size, int x, int y)
{
  int status = 0;
  int offset = 0;

  for (auto c = str.begin(); c != str.end(); ++c)
  {
    const CharFont::char_font * image = CharFont::getChar(*c, size);
    status = AddChar(image, x + offset, y);

    if (status < 0) break;

    offset += image->width + image->width / 5;
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

  const CharFont::char_font * image = CharFont::getChar(c, size);

  return AddChar(image, x, y);
}

int BitImage::AddChar(const CharFont::char_font * image, int x, int y)
{

  if (image == NULL) return -2;
  if (image->width + x >= width) return -3;
  if (image->height + y >= height) return -4;

  int xShift = x & 0x7;

  int hCount = image->height;
  int wCount = (image->width) / 8;
  unsigned char mask = 0x00;
  bool addFinalByte = false;

  if (image->width & 0x7)
  {
    int maskShift = ((image->width & 0x7) + xShift);
    if (maskShift > 8)
    {
      maskShift -= 8;
      if (maskShift) addFinalByte = true;
    }
    if (maskShift) mask = 0xFF >> maskShift;
    wCount++;
  }

  printf("Char size is %u %u\n", image->height, image->width);
  printf("X Shift is %u\n", xShift);

  int count = hCount * wCount;
  int scrWidth = width / 8;
  int wStart = x / 8;

  unsigned char maskByte = 0xFF << (8 - xShift);
  unsigned char nextByte = maskByte & buffer[wStart];

  for (int i = 0; i < count; i++)
  {
    unsigned char byte = image->data[i];
    printf("Byte is 0x%02X\n", byte);

    if (xShift == 0)
    {
      buffer[wStart + i % wCount] = byte;
      printf("Updated %u with 0x%02X\n", wStart + i % wCount, byte);
    }
    else
    {
      unsigned char tempByte = byte >> xShift;
      printf("Updated %u with 0x%02X | 0x%02X\n", wStart + i % wCount, tempByte, nextByte);
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
