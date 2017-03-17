#ifndef bitimage_fonts_h
#define bitimage_fonts_h

#include <string>
#include <map>
#include <ft2build.h>
#include "bitchar.h"
#include FT_FREETYPE_H

using namespace std;

namespace CharFont
{

/*
extern const char_font * NUMBER_CHARS[10][3];
extern const char_font * LOWER_CHARS[26][3];
extern const char_font * UPPER_CHARS[26][3];
*/
class Fonts
{
private:
  map<string, FT_Face> loaded_fonts;
  map<string, BitChar> fonts_by_size;
  int dpi;

public:

  Fonts() {}
  int Init(int DPI);

  int Load(string font_path, string font_name);

  int GetChar(string font_name, int size, char c, BitCharBuffer *& buffer);
};

/*
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
*/

};

#endif
