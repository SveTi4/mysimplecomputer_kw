#include "myALU.h"
#include "mySimpleComputer.h"

int
ALU (int command, int operand)
{
  mi_showCursor ();
  int tmp;
  sc_memoryGet (operand, &tmp);
  switch (command)
    {
    case ADD:
      accumulator += tmp;
      break;
    case SUB:
      accumulator -= tmp;
      break;
    case DIVIDE:
      accumulator /= tmp;
      break;
    case MUL:
      accumulator *= tmp;
      break;
    default:
      sc_regSet (FLAG_E, true);
      return -1;
    }
  return 0;
}