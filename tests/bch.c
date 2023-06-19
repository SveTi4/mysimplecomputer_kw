#include "myBigChars.h"
#include "mySimpleComputer.h"
#include "myTerm.h"
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

extern int instruction_counter;

void
testFONT (int num, int x, int y)
{
  bc_printBigChar (big_chars[num], x, y, RED, DEFAULT);
  mt_gotoXY (x + 1, y + 12);
}

int
main ()
{
  int count, fd;
  int big[2];
  mt_clrscr ();
  testFONT (2, 4, 2);
  bc_printBigChar (big_chars[16], 14, 2, WHITE, DEFAULT);
  testFONT (2, 23, 2);
  bc_printBigChar (big_chars[17], 33, 0, WHITE, DEFAULT);
  bc_printBigChar (big_chars[17], 33, 4, WHITE, DEFAULT);
  testFONT (4, 43, 2);
  printf ("\tшрифт ↓\n");
  fd = open ("mybch.font", O_RDONLY);
  if (fd < 0)
    {
      perror ("open");
      abort ();
    }
  for (int i = 0; i < 18; i++)
    {
      if (bc_bigCharRead (fd, big, 2, &count) < 0)
        {
          perror ("bc_bigcharread");
          abort ();
        }
      if (count != 2 || big[0] != big_chars[i][0] || big[1] != big_chars[i][1])
        {
          fprintf (stderr, "ошибка: чтение неожиданного большого символа %d\n",
                   i);
          abort ();
        }
      bc_printBigChar (big, 2 + i * 10, 15, BLUE, DEFAULT);
    }
  if (close (fd) < 0)
    {
      perror ("close");
      abort ();
    }
  printf ("\nвсе тесты прошли успешно!\n");

  return 0;
}