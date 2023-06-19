#include "myReadKey.h"

struct termios save;

int
rk_readKey (enum keys *key)
{
  fflush (stdout); // очистка потока вывода
  char buffer[5] = { 0 };
  rk_myTermRegime (0, 30, 0, 0, 0);
  read (fileno (stdin), buffer, sizeof (buffer));
  rk_myTermRestore ();

  if (key == NULL)
    {
      return -1;
    }

  if (buffer[0] == '\033')
    { // Esc-последовательности
      switch (buffer[1])
        {
        case '\0':
          *key = ESC_KEY;
          break;
        case '[':
          switch (buffer[2])
            {
            case 'A':
              *key = UP_KEY;
              break;
            case 'B':
              *key = DOWN_KEY;
              break;
            case 'C':
              *key = RIGHT_KEY;
              break;
            case 'D':
              *key = LEFT_KEY;
              break;
            case '1':
              if (buffer[3] == '5' && buffer[4] == '~')
                *key = F5_KEY;
              if (buffer[3] == '7' && buffer[4] == '~')
                *key = F6_KEY;
              break;
            default:
              *key = INVALID_KEY;
              break;
            }
          break;
        default:
          *key = INVALID_KEY;
          break;
        }
    }
  else if (buffer[0] == '\n' && buffer[1] == '\0')
    { // клавиша Enter
      *key = ENTER_KEY;
    }
  else
    { // прочие клавиши
      switch (buffer[0])
        {
        case 'l':
        case 'L':
          *key = L_KEY;
          break;
        case 's':
        case 'S':
          *key = S_KEY;
          break;
        case 'r':
        case 'R':
          *key = R_KEY;
          break;
        case 't':
        case 'T':
          *key = T_KEY;
          break;
        case 'i':
        case 'I':
          *key = I_KEY;
          break;
        case 0:
          *key = NOTHING_KEY;
          break;
        default:
          *key = INVALID_KEY;
          break;
        }
    }

  return 0;
}

int
rk_myTermSave ()
{
  if (tcgetattr (fileno (stdin), &save))
    return -1;
  return 0;
}

int
rk_myTermRestore ()
{
  tcsetattr (fileno (stdin), TCSAFLUSH, &save);
  return 0;
}

int
rk_myTermRegime (int regime, int vtime, int vmin, int echo, int sigint)
{
  struct termios curr;
  tcgetattr (fileno (stdin), &curr);

  if (regime)
    curr.c_lflag |= ICANON;
  else
    {
      curr.c_lflag &= ~ICANON;
      sigint ? (curr.c_lflag |= ISIG) : (curr.c_lflag &= ~ISIG);
      echo ? (curr.c_lflag |= ECHO) : (curr.c_lflag &= ~ECHO);
      curr.c_cc[VMIN] = vmin;
      curr.c_cc[VTIME] = vtime;
    }
  tcsetattr (0, TCSAFLUSH, &curr);
  return 0;
}