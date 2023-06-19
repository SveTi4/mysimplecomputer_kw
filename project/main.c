#include <myInterface.h>
#include <myReadKey.h>
#include <mySignal.h>
#include <mySimpleComputer.h>
#include <myTerm.h>

bool on = true;

int
main ()
{
  sc_halt = false;
  enum keys key;
  struct itimerval nval, oval;
  rk_myTermSave ();
  bc_fontInit ("mybch.font");
  sc_memoryInit ();
  for (int i = 0; i < RAM_SIZE; i++)
    sc_memorySet (i, i + 1);
  mi_uiInit ();
  ms_setTimer (&nval, &oval);
  do
    {
      mi_uiUpdate (sc_halt);
      mi_hideCursor ();
      rk_readKey (&key);
      switch (key)
        {
        case UP_KEY:
        case RIGHT_KEY:
        case DOWN_KEY:
        case LEFT_KEY:
          mi_currMemMove (key);
          break;
        case L_KEY:
          mi_dirMenu ();
          break;
        case S_KEY:
          mi_memorySave ();
          break;
        case R_KEY:
          sc_regInit ();
          setitimer (ITIMER_REAL, &nval, &oval);
          break;
        case T_KEY:
          mc_oneTactPulse ();
          break;
        case I_KEY:
          sc_restart ();
          break;
        case F5_KEY:
          mi_accum ();
          break;
        case F6_KEY:
          mi_counter ();
          break;
        case ENTER_KEY:
          mi_uiSetValue ();
          break;
        case NOTHING_KEY:
        case INVALID_KEY:
          sc_halt = true;
          break;
        case ESC_KEY:
          sc_halt = false;
          on = false;
          break;
        }
    }
  while (on);
  mt_clrscr ();
  rk_myTermRestore ();
  mi_showCursor ();
  return 0;
}