#include "utf8iterator.h"
#include <iostream>

using namespace std;

utf8iterator::utf8iterator(string & str) :
  base(&str), iter(base->begin()), curChar(0)
{
  nextChar(true);
}

utf8iterator & utf8iterator::operator++()
{
  nextChar(false);
  return *this;
}

void utf8iterator::nextChar(bool isInit)
{
  curChar = 0;
  uint32_t character = 0;
  int count = 0;

  if (not isInit) ++iter;

  while (iter != base->end())
  {
    unsigned char c = *iter;

    cout << "--" << std::hex << (unsigned int)c << endl;

    if (count == 0)
    {
      if ((c & 0xF8) == 0xF0)
      {
        count = 3;
        character = uint32_t(c & 0x7);
      }
      else if ((c & 0xF0) == 0xE0)
      {
        count = 2;
        character = uint32_t(c & 0xF);
      }
      else if ((c & 0xE0) == 0xC0)
      {
        count = 1;
        character = uint32_t(c & 0x1F);
      }
      else
      {
        character = uint32_t(c);
      }

      if (count)
      {
        cout << "++" << count << endl;
        character <<= (count * 6);
      }

      //character = uint32_t(c) << shift;
    }
    else
    {
      --count;
      /*
      character >>= 8;
      character |= uint32_t(c) << shift;
      */
      character |= uint32_t(c & 0x3F) << (6 * count);
    }

    if (count == 0)
    {
      curChar = character;
      break;
    }

    ++iter;
  }
}

uint32_t utf8iterator::operator* ()
{
  return curChar;
}

bool utf8iterator::operator!= (const string::iterator other)
{
  return iter != other;
}
