#include "bitimage.h"
#include "bitimage_fonts.h"

bool BitImage::alloc()
{
  if (buffer) delete buffer;

  buffer = NULL;

  if (width != 0 or height != 0)
  {
    buffer = new unsigned char[width * height];
  }

  if (buffer)
  {
    for (int i = 0; i < width * height; ++i)
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

  for (int vert = 0; vert < image->height; ++vert)
  {
    int bufoffset = (y + vert) * height;
    int imageoffset = vert * image->height;
    for (int hor = 0; hor < image->width; ++hor)
    {
      unsigned char byte = image->data[imageoffset];
      buffer[bufoffset] = byte;
    }
  }

  return 1;
}
