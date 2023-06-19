#include "myTerm.h"
#include <string.h>

int
mt_clrscr ()
{
  const char *str = "\E[H\E[2J";
  write (STDOUT_FILENO, str, strlen (str));
  return 0;
}

int
mt_gotoXY (int x, int y)
{
  int count_rows, count_columns;
  if (mt_getScreenSize (&count_rows, &count_columns) == -1)
    {
      return -1;
    }
  if ((x > count_columns) || (x <= 0) || (y > count_rows) || (y <= 0))
    {
      return -1;
    }
  char buf[16];
  int len = sprintf (buf, "\033[%d;%dH", y, x);
  write (STDOUT_FILENO, buf, len);
  return 0;
}

int
mt_getScreenSize (int *rows, int *columns)
{
  struct winsize window_size;
  if (ioctl (1, TIOCGWINSZ, &window_size))
    {
      return -1;
    }
  *rows = window_size.ws_row;
  *columns = window_size.ws_col;
  return 0;
}

int
mt_setFgColor (enum colors color)
{
  char buf[16];
  int len = sprintf (buf, "\033[38;5;%dm", color);
  write (STDOUT_FILENO, buf, len);
  return 0;
}

int
mt_setBgColor (enum colors color)
{
  char buf[16];
  int len = sprintf (buf, "\033[48;5;%dm", color);
  write (STDOUT_FILENO, buf, len);
  return 0;
}

int
mt_setDfColor ()
{
  const char *str = "\033[0m";
  write (STDOUT_FILENO, str, strlen (str));
  return 0;
}

int
mt_printSymbol (char symbol)
{
  char buf[16];
  int len = sprintf (buf, "\033(0%c\033(B", symbol);
  write (STDOUT_FILENO, buf, len);
  return 0;
}
