#include "myBigChars.h"

int font[36];
int big_chars[][2] = {
  { 0xE7E7FFFF, 0xFFFFE7E7 }, // 0
  { 0x1CDC7C3C, 0xFFFF1C1C }, // 1
  { 0xFF07FFFF, 0xFFFFE0FF }, // 2
  { 0xFF07FFFF, 0xFFFF07FF }, // 3
  { 0xFFE7E7E7, 0x070707FF }, // 4
  { 0xFFE0FFFF, 0xFFFF07FF }, // 5
  { 0xFFE0FFFF, 0xFFFFE7FF }, // 6
  { 0x1C0EFFFE, 0x3838FE38 }, // 7
  { 0x7EE7FF7E, 0x7EFFE77E }, // 8
  { 0xFFE7FFFF, 0xFFFF07FF }, // 9
  { 0xFFE7FF7E, 0xE7E7E7FF }, // A
  { 0xFEE7FFFE, 0xFEFFE7FE }, // B
  { 0xE0E7FF7E, 0x7EFFE7E0 }, // C
  { 0xE7E7FFF8, 0xF8FFE7E7 }, // D
  { 0xFFE0FFFF, 0xFFFFE0FF }, // E
  { 0xFFE0FFFF, 0xE0E0E0FF }, // F
  { 0x7E180000, 0x00000018 }, // +
  { 0x7E000000, 0x00000000 }, // -
};

int
bc_printA (char charr)
{
  char buf[8];
  sprintf (buf, "\033(0%c\033(B", charr);
  write (1, buf, strlen (buf));
  return 0;
}

int
bc_box (int x, int y, int width, int height)
{
  int rows, cols;
  mt_getScreenSize (&rows, &cols);
  if ((x <= 0) || (y <= 0) || (x + width - 1 > cols) || (y + height - 1 > rows)
      || (width <= 1) || (height <= 1))
    return -1;

  mt_gotoXY (x, y);
  bc_printA ((char)ACS_ULCORNER);
  mt_gotoXY (x + width - 1, y);
  bc_printA ((char)ACS_URCORNER);
  mt_gotoXY (x + width - 1, y + height - 1);
  bc_printA ((char)ACS_LRCORNER);
  mt_gotoXY (x, y + height - 1);
  bc_printA ((char)ACS_LLCORNER);

  for (int i = 1; i < width - 1; ++i)
    {
      mt_gotoXY (x + i, y);
      bc_printA ((char)ACS_HLINE);
      mt_gotoXY (x + i, y + height - 1);
      bc_printA ((char)ACS_HLINE);
    }
  for (int i = 1; i < height - 1; ++i)
    {
      mt_gotoXY (x, y + i);
      bc_printA ((char)ACS_VLINE);
      mt_gotoXY (x + width - 1, y + i);
      bc_printA ((char)ACS_VLINE);
    }
  return 0;
}

int
bc_printBigChar (int *big, int x, int y, enum colors colorFG,
                 enum colors colorBG)
{
  char buf[6];
  if (colorFG != DEFAULT)
    mt_setFgColor (colorFG);
  if (colorBG != DEFAULT)
    mt_setBgColor (colorBG);

  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      {
        mt_gotoXY (x + i, y + j);
        int value;
        if (bc_getBigCharPos (big, i, j, &value))
          return -1;
        if (value)
          bc_printA (ACS_CKBOARD);
        if (!value)
          {
            snprintf (buf, 6, "%c", ' ');
          }
        write (1, buf, strlen (buf));
      }

  mt_setDfColor ();
  return 0;
}

int
bc_setBigCharPos (int *big, int x, int y, int value)
{
  if ((x < 0) || (x > 7) || (y < 0) || (y > 7))
    return -1;
  if (value)
    big[(int)(y / 4)] |= (1 << (8 * (y % 4) + (7 - x)));
  else
    big[(int)(y / 4)] &= ~(1 << (8 * (y % 4) + (7 - x)));
  return 0;
}

int
bc_getBigCharPos (int *big, int x, int y, int *value)
{
  if ((x < 0) || (x > 7) || (y < 0) || (y > 7))
    return -1;
  if ((big[(int)(y / 4)] & (1 << (8 * (y % 4) + (7 - x)))) != 0)
    *value = 1;
  else
    *value = 0;
  return 0;
}

int
bc_bigCharWrite (int fd, int *big, int count)
{
  if (big == NULL || count <= 0)
    {
      return -1;
    }
  ssize_t bytes_written = write (fd, big, count * sizeof (int));
  if (bytes_written == -1)
    {
      return -1;
    }
  return 0;
}

int
bc_bigCharRead (int fd, int *big, int need_count, int *count)
{
  if (big == NULL || need_count <= 0)
    {
      return -1;
    }
  ssize_t bytes_read = read (fd, big, need_count * sizeof (int));
  if (bytes_read == -1)
    {
      return -1;
    }
  if (count != NULL)
    {
      *count = bytes_read / sizeof (int);
    }
  return 0;
}

void
bc_fontInit (char *filename)
{
  int fd = open (filename, O_RDONLY);
  if (fd < 0)
    {
      perror ("open");
      abort ();
    }
  int count;
  if (bc_bigCharRead (fd, font, 36, &count) < 0)
    {
      perror ("bc_bigcharread");
      abort ();
    }
  if (close (fd) < 0)
    {
      perror ("close");
      abort ();
    }
}