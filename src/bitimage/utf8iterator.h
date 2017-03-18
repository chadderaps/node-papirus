#include <string>

using namespace std;

class utf8iterator
{
private:
  string * base;
  string::iterator iter;
  uint32_t curChar;

  void nextChar(bool isInit);

public:
  utf8iterator(string & str);

  utf8iterator & operator++();
  uint32_t operator* ();
  bool operator!= (const string::iterator other);
};
