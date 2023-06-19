#include "myInterface.h"
#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"
#include <signal.h>

bool resize = false;

int
mi_displayMemoryValues ()
{
  for (int i = 0; i < 10; ++i)
    {
      for (int j = 0; j < 10; ++j)
        {
          mt_gotoXY (2 + (5 * j + j), 2 + i);
          int value;
          sc_memoryGet (i * 10 + j, &value);
          if ((i * 10 + j) == currMemCell && !sc_halt)
            mt_setBgColor (PURPLE);
          else
            mt_setDfColor ();
          int command, operand;
          sc_commandDecode (value, &command, &operand);
          char buffer[7];
          int length
              = sprintf (buffer, "%c%02X%02X", (value >> 14) ? '-' : '+',
                         abs ((value >> 7) & 0x7F), abs (value & 0x7F));
          write (STDOUT_FILENO, buffer, length);
        }
    }
  mt_setDfColor ();
  return 0;
}

int
mi_displayAccumulator ()
{
  char buf[6];
  mt_gotoXY (70, 2);
  if (sc_halt)
    mt_setBgColor (GREEN);
  if ((accumulator >> 14) & 0x1)
    sprintf (buf, "-%04X", accumulator & 0x3fff);
  else
    sprintf (buf, "+%04X", accumulator & 0x3fff);
  write (STDOUT_FILENO, buf, 5);
  mt_setDfColor ();
  return 0;
}

int
mi_displayInstructionCounter ()
{
  char buffer[6];
  mt_gotoXY (70, 5);
  sprintf (buffer, "+%04d", instruction_counter);
  write (STDOUT_FILENO, buffer, sizeof (buffer));
  return 0;
}

int
mi_displayOperation ()
{
  int value = 0, command = 0, operand = 0;
  if (sc_memoryGet (instruction_counter, &value))
    return -1;
  if (sc_commandDecode (value, &command, &operand))
    {
      value = 0;
      command = 0;
      operand = 0;
    }
  mt_gotoXY (68, 8);
  char buf[32];
  char sign = (value != 0) ? '+' : '-';
  sprintf (buf, " %c%02X : %02X", sign, command, operand);
  write (STDOUT_FILENO, buf, strlen (buf));
  return 0;
}

int
mi_displayFlags ()
{
  char flags[] = { 'P', '0', 'M', 'T', 'E' };
  for (int i = 0; i < REG_COUNT; i++)
    {
      int value;
      sc_regGet (i + 1, &value);
      if (value == 1)
        mt_setFgColor (RED);
      else
        mt_setFgColor (NONACTIVE);
      mt_gotoXY (68 + (i * 2), 11);
      char buffer[2];
      int length = sprintf (buffer, "%c", flags[i]);
      write (STDOUT_FILENO, buffer, length);
    }
  mt_setDfColor ();
  return 0;
}

int
mi_displayBoxes ()
{
  bc_box (MEMORY_X, MEMORY_Y, MEMORY_WIDTH, MEMORY_HEIGHT);
  bc_box (ACC_X, ACC_Y, ACC_WIDTH, ACC_HEIGHT);
  bc_box (COUNTER_X, COUNTER_Y, COUNTER_WIDTH, COUNTER_HEIGHT);
  bc_box (OP_X, OP_Y, OP_WIDTH, OP_HEIGHT);
  bc_box (FLAGS_X, FLAGS_Y, FLAGS_WIDTH, FLAGS_HEIGHT);
  bc_box (BIG_CHARS_X, BIG_CHARS_Y, BIG_CHARS_WIDTH, BIG_CHARS_HEIGHT);
  bc_box (KEYS_X, KEYS_Y, KEYS_WIDTH, KEYS_HEIGHT);
  return 0;
}

int
mi_displayTexts ()
{
  mt_gotoXY (27, 1);
  write (STDOUT_FILENO, " Memory ", strlen (" Memory "));
  mt_gotoXY (66, 1);
  write (STDOUT_FILENO, " accumulator ", strlen (" accumulator "));
  mt_gotoXY (63, 4);
  write (STDOUT_FILENO, " instructionCounter ",
         strlen (" instructionCounter "));
  mt_gotoXY (67, 7);
  write (STDOUT_FILENO, " Operation ", strlen (" Operation "));
  mt_gotoXY (68, 10);
  write (STDOUT_FILENO, " Flags ", strlen (" Flags "));
  mt_gotoXY (54, 13);
  write (STDOUT_FILENO, " Keys: ", strlen (" Keys: "));
  char *hot_keys[7] = { (char *)" l  – load",
                        (char *)" s  – save",
                        (char *)" r  – run",
                        (char *)" t  – step",
                        (char *)" i  – reset",
                        (char *)" F5 – accumulator",
                        (char *)" F6 – instructionCounter" };

  for (int i = 0; i < 7; i++)
    {
      mt_gotoXY (54, i + 14);
      write (STDOUT_FILENO, hot_keys[i], strlen (hot_keys[i]));
    }
  mt_gotoXY (1, 24);
  return 0;
}

int
mi_displayBigChars ()
{
  int value, command, operand;
  short ind;
  if (sc_memoryGet (currMemCell, &value))
    return -1;

  if (!(value >> 14))
    {
      ind = 16;
      bc_printBigChar (&font[ind * 2], 2, 14, PURPLE, 0);
    }
  else
    {
      ind = 17;
      bc_printBigChar (&font[ind * 2], 2, 14, PURPLE, 0);
    }
  sc_commandDecode (value & 0x3FFF, &command, &operand);
  int ch;
  for (int i = 0; i < 4; ++i)
    {
      switch (i)
        {
        case 0:
          ch = (command >> 4) & 0xF;
          break;
        case 1:
          ch = (command)&0xF;
          break;
        case 2:
          ch = (operand >> 4) & 0xF;
          break;
        case 3:
          ch = (operand)&0xF;
          break;
        }
      bc_printBigChar (&font[ch * 2], 2 + 8 * (i + 1) + 2 * (i + 1), 14,
                       PURPLE, 0);
    }

  return 0;
}

int
mi_displayAccumulatorBigChars ()
{
  if ((accumulator >> 14) & 0x1)
    {
      bc_printBigChar (&font[17 * 2], 2, 14, GREEN, 0);
    }
  else
    {
      bc_printBigChar (&font[16 * 2], 2, 14, GREEN, 0);
    }
  for (int i = 0; i < 4; ++i)
    {
      int digit = ((accumulator & 0x3FFF) >> ((3 - i) * 4)) & 0xF;
      bc_printBigChar (&font[digit * 2], 2 + 8 * (i + 1) + 2 * (i + 1), 14,
                       GREEN, 0);
    }
  return 0;
}

int
mi_counter ()
{
  sc_halt = false;
  char buffer[32];
  write (STDOUT_FILENO, "\033[2K", 4);
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO,
         "Enter inctruntionCounter: ", strlen ("Enter inctruntionCounter: "));
  fgets (buffer, 32, stdin);
  if (atoi (buffer) < 0 || atoi (buffer) > 99 || instruction_counter < 0
      || instruction_counter > 99)
    {
      mt_gotoXY (1, 24);
      mi_messageOutput (
          (char *)"Значение incrtuctionCounter не должно превышать 99!!!",
          ERROR);
      return 1;
    }
  else
    instruction_counter = atoi (buffer);
  mi_displayInstructionCounter ();
  return 0;
}

int
mi_accum ()
{
  sc_halt = true;
  mi_uiUpdate (sc_halt);
  char buf[10];
  char *pEnd;
  write (STDOUT_FILENO, "\033[2K", 4);
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO, "Enter accumulator: ", strlen ("Enter accumulator: "));
  int n = read (0, buf, 10);
  if (n != 6)
    {
      mt_gotoXY (1, 24);
      mi_messageOutput (
          (char *)"Accumulator имеет знак <+/-> и 4 бита значения", YELLOW);
      return -1;
    }
  buf[5] = '\0';
  for (int i = 1; i < 5; ++i)
    {
      if ((buf[i] < '0' || buf[i] > '9') && (buf[i] < 'a' || buf[i] > 'f')
          && (buf[i] < 'A' || buf[i] > 'F'))
        {
          mt_gotoXY (1, 24);
          mi_messageOutput (
              (char *)"Accumulator принимает значения только от 0-F", RED);
          return -1;
        }
    }
  int temp = strtol (&buf[1], &pEnd, 16);
  if (temp > 0x3fff)
    {
      mt_gotoXY (1, 24);
      mi_messageOutput (
          (char *)"Значение accumulator не должно превышать 14 бит (0x3FFF)",
          RED);
      return -1;
    }
  if (buf[0] == '+')
    accumulator = temp;
  else if (buf[0] == '-')
    {
      accumulator = temp;
      accumulator |= 0x1 << 14;
    }
  else
    {
      mt_gotoXY (1, 24);
      mi_messageOutput ((char *)"Accumulator должен иметь знак только + или -",
                        RED);
      return -1;
    }
  //    halt = false;
  return 0;
}

int
mi_uiInit ()
{
  currMemCell = 0;
  instruction_counter = 0;
  int count_rows, count_columns;
  mt_getScreenSize (&count_rows, &count_columns);
  if (count_rows < 30 || count_columns < 30)
    {
      mt_clrscr ();
      mi_messageOutput ((char *)"Маленький размер окна!!!", ERROR);
      mi_messageOutput ((char *)"Увеличьте размер окна!!!", YELLOW);
      resize = false;
      return -1;
    }
  else
    {
      mt_clrscr ();
      mi_displayBoxes ();
      mi_displayTexts ();
    }
  return 0;
}

int
mi_uiUpdate (bool halt)
{
  int count_rows, count_columns;
  mt_getScreenSize (&count_rows, &count_columns);

  if (count_rows < 30 || count_columns < 30)
    {
      if (!mi_uiInit () || !resize)
        resize = true;
    }
  else if (count_rows > 30 && count_columns > 30 && resize)
    {
      mt_clrscr ();
      mi_displayBoxes ();
      mi_displayTexts ();
      resize = false;
      goto ignoreTactUpdate;
    }
  else
    {
    ignoreTactUpdate:
      mi_displayTexts ();
      mi_displayAccumulator ();
      mi_displayInstructionCounter ();
      mi_displayOperation ();
      mi_displayFlags ();
      mi_displayMemoryValues ();
      mi_displayBigChars ();
      if (halt)
        {
          mi_displayAccumulatorBigChars ();
        }
      mt_gotoXY (1, 24);
      write (STDOUT_FILENO, "\033[2K", 4);
      write (STDOUT_FILENO, "Input/Output: ", strlen ("Input/Output: "));
    }
  return 0;
}

int
mi_uiSetValue ()
{
  mi_showCursor ();
  char buffer[8], inv[32];
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO, "\r\E[K", 4);
  sprintf (inv, "\033[38;5;%dmInput/Output: \033[0m", CYAN);
  write (STDOUT_FILENO, inv, 32);
  fgets (buffer, 8, stdin);
  mt_gotoXY (8, 24);
  write (STDOUT_FILENO, "\033[2K", 4);

  if (buffer[strlen (buffer) - 1] != '\n')
    mi_clearBuffIn ();

  if (!mi_checkCorrectInput (buffer))
    {
      mt_gotoXY (1, 24);
      mi_messageOutput ((char *)"Недопустимый ввод", ERROR);
      return -1;
    }

  long int number = 0;
  char *tmp;

  if (buffer[0] == '+')
    {
      number = strtol (&buffer[1], &tmp, 16);
      if (number > 0x3FFF)
        {
          mi_messageOutput (
              (char *)"Значение команды не должно превышать 14 бит (0x3FFF)",
              RED);
          return -1;
        }
      sc_memorySet (currMemCell, number);
    }
  else if (buffer[0] != '-')
    {
      number = strtol (buffer, &tmp, 16);
      if (number > 0x3FFF)
        {
          mi_messageOutput (
              (char *)"Значение команды не должно превышать 14 бит (0x3FFF)",
              RED);
          return -1;
        }
      sc_memorySet (currMemCell, number);
    }
  else if (buffer[0] == '-')
    {
      number = strtol (&buffer[1], &tmp, 16);
      if (number > 0x3FFF)
        {
          mi_messageOutput (
              (char *)"Значение команды не должно превышать 14 бит (0x3FFF)",
              RED);
          return -1;
        }
      number = number | 0x4000;
      sc_memorySet (currMemCell, number);
    }
  mi_hideCursor ();
  return 0;
}

bool
mi_checkCorrectInput (const char buffer[10])
{
  int i;
  if (buffer[0] == '+' || buffer[0] == '-')
    {
      if (strlen (buffer) == 2 || strlen (buffer) > 6)
        return false;
      i = 1;
    }
  else
    {
      i = 0;
      if (strlen (buffer) == 1 || strlen (buffer) > 5)
        return false;
    }
  for (; i < strlen (buffer) - 1; ++i)
    if (!(isxdigit (buffer[i])))
      return false;

  return true;
}

int
mi_messageOutput (char *str, enum colors color)
{

  char buff[100] = { 0 };
  sprintf (buff, "\033[38;5;%dm%s\033[0m", color, str);
  write (STDOUT_FILENO, buff, 100);
  color == ERROR ? sleep (1) : sleep (2);
  write (STDOUT_FILENO, "\033[2K", 4);
  return 0;
}

int
mi_clearBuffIn ()
{
  int c;
  do
    {
      c = getchar ();
    }
  while (c != '\n' && c != '\0');
  return 0;
}

int
mi_hideCursor ()
{
  write (STDOUT_FILENO, "\E[?25l\E[?1c", 11);
  return 0;
}

int
mi_showCursor ()
{
  sc_halt = false;
  write (STDOUT_FILENO, "\E[?25h\E?8c", 11);
  return 0;
}

void
mi_dirMenu ()
{
  DIR *dir;
  struct dirent *ent;
  struct stat st;
  char dir_path[] = "resources/";
  int selected = 0;
  int total = 0;
  sc_halt = false;
  raise (SIGUSR1);
  if ((dir = opendir (dir_path)) != NULL)
    {
      while ((ent = readdir (dir)) != NULL)
        {
          char path[100] = "";
          strcat (path, dir_path);
          strcat (path, ent->d_name);
          stat (path, &st);
          if (S_ISREG (st.st_mode))
            {
              // Проверяем, что имя файла не начинается с символа точки
              if (ent->d_name[0] != '.')
                {
                  total++;
                }
            }
        }
      while (1)
        {
          rewinddir (dir);
          int count = 0;
          mt_clrscr ();
          mt_gotoXY (1, 1);
          printf ("Выберите файл сохранения:\n\n");
          while ((ent = readdir (dir)) != NULL)
            {
              char path[100] = "";
              strcat (path, dir_path);
              strcat (path, ent->d_name);
              stat (path, &st);
              if (S_ISREG (st.st_mode))
                {
                  // Проверяем, что имя файла не начинается с символа точки
                  if (ent->d_name[0] != '.')
                    {
                      if (count == selected)
                        {
                          printf ("> \033[7m%s\033[0m\n", ent->d_name);
                        }
                      else
                        {
                          printf ("> %s\n", ent->d_name);
                        }
                      count++;
                    }
                }
            }
          enum keys key;
          rk_readKey (&key);
          if (key == ESC_KEY)
            { // ESC
              mt_clrscr ();
              mi_uiInit ();
              mi_uiUpdate (sc_halt);
              break;
            }
          else if (key == ENTER_KEY)
            { // ENTER
              rewinddir (dir);
              count = 0;
              while ((ent = readdir (dir)) != NULL)
                {
                  char path[100] = "";
                  strcat (path, dir_path);
                  strcat (path, ent->d_name);
                  stat (path, &st);
                  if (S_ISREG (st.st_mode))
                    {
                      // Проверяем, что имя файла не начинается с символа точки
                      if (ent->d_name[0] != '.')
                        {
                          if (count == selected)
                            {
                              mt_clrscr ();
                              mi_uiInit ();
                              sc_restart ();
                              mi_displayInstructionCounter ();
                              mi_uiUpdate (sc_halt);
                              sc_memoryLoad (path);
                              return;
                            }
                          count++;
                        }
                    }
                }
            }
          else if (key == UP_KEY)
            { // UP
              selected--;
              if (selected < 0)
                {
                  selected = total - 1;
                }
            }
          else if (key == DOWN_KEY)
            { // DOWN
              selected++;
              if (selected >= total)
                {
                  selected = 0;
                }
            }
        }
      closedir (dir);
    }
  else
    {
      perror ("");
    }
}

int
mi_memorySave ()
{
  sc_halt = false;
  char buf[100];
  memset (buf, 0, sizeof (buf));
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO, "\r\E[K", 4);
  write (STDOUT_FILENO, "Filename> ", 11);
  int n = read (0, buf, 100);
  buf[n - 1] = '\0';
  if (buf[0] == '\E' && buf[1] == '\0')
    {
      mt_gotoXY (1, 24);
      write (STDOUT_FILENO, "\r\E[K", 4);
      return 1;
    }
  char path[100 + 11];
  memset (path, 0, sizeof (path));
  strcat (path, "resources/");
  strcat (path, buf);
  strcat (path, ".bin");
  if (sc_memorySave (path))
    {
      mi_messageOutput ("2", RED);
      return 1;
    }
  mi_messageOutput ("Файл успешно сохранен", GREEN);
  return 0;
}

int
mi_currMemMove (enum keys direction)
{
  sc_halt = false;
  mt_gotoXY (currMemCell / 10 + 2, currMemCell % 10 * 6 + 2);
  switch (direction)
    {
    case UP_KEY:
      currMemCell += currMemCell < 10 ? 90 : -10;
      break;
    case DOWN_KEY:
      currMemCell += currMemCell >= 90 ? -90 : 10;
      break;
    case LEFT_KEY:
      currMemCell += currMemCell % 10 == 0 ? 9 : -1;
      break;
    case RIGHT_KEY:
      currMemCell += currMemCell % 10 == 9 ? -9 : 1;
      break;
    default:
      return -1;
    }
  return 0;
}
