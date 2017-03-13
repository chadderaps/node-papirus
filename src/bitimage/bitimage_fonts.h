#ifndef bitimage_fonts_h
#define bitimage_fonts_h

#include <string>

namespace CharFont
{

struct char_font {
  int width;
  int height;
  int size;
  unsigned char * data;
};

extern const char_font * NUMBER_CHARS[10][3];
extern const char_font * LOWER_CHARS[26][3];
extern const char_font * UPPER_CHARS[26][3];

inline const char_font * getChar(char c, int size)
{
  const char_font ** search = NULL;
  const char_font * ret = NULL;

  if (c >= '0' and c <= '9')
  {
    search = NUMBER_CHARS[int(c - '0')];
  }
  else if (c >= 'a' and c <= 'z')
  {
    search = LOWER_CHARS[int(c - 'a')];
  }
  else if (c >= 'A' and c <= 'Z')
  {
    search = UPPER_CHARS[int(c - 'A')];
  }

  if (search)
  {
    for (int i = 0; i < 3; i++)
    {
      if (search[i]->size == size)
      {
        ret = search[i];
        break;
      }
    }
  }

  return ret;
}

};

#endif
