#include <myReadKey.h>
#include <mySimpleComputer.h>
bool on = 1;

int
main ()
{
  sc_memoryInit ();
  printf ("\n>> инициализирована память\n");
  rk_myTermSave ();

  enum keys key;
  do
    {
      rk_readKey (&key);
      rk_myTermRestore ();
      switch (key)
        {
        case UP_KEY:
          printf ("нажата ↑\n");
          break;
        case RIGHT_KEY:
          printf ("нажата →\n");
          break;
        case DOWN_KEY:
          printf ("нажата ↓\n");
          break;
        case LEFT_KEY:
          printf ("нажата ←\n");
          break;
        case L_KEY:
          printf ("нажата L\n");
          break;
        case S_KEY:
          printf ("нажата S\n");
          break;
        case R_KEY:
          printf ("нажата R\n");
          break;
        case T_KEY:
          printf ("нажата T\n");
          break;
        case I_KEY:
          printf ("нажата I\n");
          break;
        case F5_KEY:
          printf ("нажата F5\n");
          break;
        case F6_KEY:
          printf ("нажата F6\n");
          break;
        case ENTER_KEY:
          printf ("нажато Enter\n");
          break;
        case ESC_KEY:
          printf ("нажато Esc\n");
          on = 0;
          break;
        case INVALID_KEY:
          printf ("не та кнопка\n");
          break;
        case NOTHING_KEY:
          printf ("ничего не нажато\n");
          break;
        default:
          break;
        }
      rk_myTermRegime (0, 5, 1, 0, 0);
      key = NOTHING_KEY;
    }
  while (on);
  return 0;
}