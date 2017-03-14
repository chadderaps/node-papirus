#include <stdio.h>
#include "bitimage.h"

int main()
{
  BitImage * screen = new BitImage();

  screen->Init("Stuff and That", 24, 24);

  int ret = screen->SetChar('a', 8, 0, 0);

  const unsigned char * buffer = screen->Buffer();

  for (int i = 0; i < screen->Size(); ++i)
  {
    printf("0x%02X ", buffer[i]);
  }

  printf("\n");

  return ret;
}
