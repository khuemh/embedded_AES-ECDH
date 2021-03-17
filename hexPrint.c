#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hexPrint.h"

void printHex(unsigned char x)
{
  if (x / 16 < 10)
  {
    printf("%c", x / 16 + '0');
  }
  if (x / 16 >= 10)
  {
    printf("%c", x / 16 - 10 + 'A');
  }
  if (x % 16 < 10)
  {
    printf("%c", x % 16 + '0');
  }
  if (x % 16 >= 10)
  {
    printf("%c", x % 16 - 10 + 'A');
  }
}

void printHex_str(uint8_t * str, uint16_t strLen)
{
  for (int i = 0; i < strLen; i++)
	{
		// printHex(str[i]);
    printf("%.2x", str[i]);

	}
}
