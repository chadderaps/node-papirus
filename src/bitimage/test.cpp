#include <stdio.h>
#include "bitimage_fonts.h"

void debug(string info)
{
  printf("%s\n", info.c_str());
}

using namespace CharFont;
int main(int argc, char * argv[])
{
  Fonts fonts = Fonts();

  if (argc != 3) return -1;

  debug("Init");
  int error = fonts.Init(96);

  if (error) printf("Error 1 %d\n", error);

  debug("Load");
  error = fonts.Load(argv[1], "Menlo");

  if (error) printf("Error 2 %d\n", error);

  BitCharBuffer * buffer = NULL;

  debug("Get Char");
  error = fonts.GetChar("Menlo", 16, argv[2][0], buffer);

  if (error) printf("Error 3 %d\n", error);
  if (buffer == NULL)
  {
    printf("Invalid BitChar Buffer\n");
    return -1;
  }

  debug("Dumping Buffer");
  for (BitCharBuffer::iterator iter = buffer->begin(); iter != buffer->end(); ++iter)
  {
    debug("Looping");
    printf("%u, %u - 0x%02X\n", iter.Col(), iter.Row(), *iter);
  }

  return 0;
}
