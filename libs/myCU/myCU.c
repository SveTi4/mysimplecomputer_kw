#include "myCU.h"

int
mc_read (int operand)
{
  rk_myTermRestore ();
  mi_showCursor ();
  char buf[10];
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO, "\033[2K", 4);
  write (STDOUT_FILENO, "Input/Output: ", strlen ("Input/Output: "));
  mt_gotoXY (1, 25);
  write (STDOUT_FILENO, "\033[2K", 4);
  int n = sprintf (buf, "%X< ", operand);
  write (STDOUT_FILENO, buf, n);
  char buffAf[32];
  fgets (buffAf, 32, stdin);
  sc_memorySet (operand, atoi (buffAf));
  rk_myTermRegime (0, 30, 0, 0, 0);
  return 0;
}

int
mc_write (int operand)
{
  int value;
  int command;
  char buf[20];
  if (sc_memoryGet (operand, &value)
      || sc_commandDecode (value & 0x3FFF, &command, &operand))
    {
      sc_regSet (FLAG_E, 1);
      return -1;
    }
  snprintf (buf, 20, "Output:> %c%02X%02X", (value & 0x4000) ? '-' : '+',
            command, operand);
  mt_gotoXY (1, 26);
  write (1, buf, 15);
  return 0;
}

int
mc_load (int operand)
{
  int value = 0;
  sc_memoryGet (operand, &value);
  accumulator = value;
  return 0;
}

int
mc_store (int operand)
{
  sc_memorySet (operand, accumulator);
  return 0;
}

int
mc_jump (int operand)
{
  instruction_counter = operand;
  CU ();
  return 0;
}

int
mc_jneg (int operand)
{
  if (accumulator < 0)
    {
      instruction_counter = operand;
      CU ();
    }
  return 0;
}

int
mc_jz (int operand)
{
  if (accumulator == 0)
    {
      instruction_counter = operand;
      CU ();
    }
  return 0;
}

int
mc_halt ()
{
  sc_regSet (FLAG_T, 1);
  return 0;
}

int
mc_addc (int operand)
{
  int value = 0;
  if (sc_memoryGet (operand, &value) == 0)
    {
      accumulator += value;
    }
  else
    {
      sc_regSet (FLAG_E, 1);
    }
  return 0;
}

int
CU ()
{
  int value = 0;
  sc_memoryGet (instruction_counter, &value);
  int command, operand;
  if (sc_commandDecode (value & 0x3FFF, &command, &operand))
    {
      sc_regSet (FLAG_E, 1);
      return -1;
    }
  if (((command >= 0x30) && (command <= 0x33)) || (command == 0x52))
    {
      ALU (command, operand);
    }
  else
    {
      switch (command)
        {
        case READ:
          mc_read (operand);
          return READ;
          break;
        case WRITE:
          mc_write (operand);
          return WRITE;
          break;
        case LOAD:
          mc_load (operand);
          return LOAD;
          break;
        case STORE:
          mc_store (operand);
          return STORE;
          break;
        case JUMP:
          mc_jump (operand);
          return JUMP;
          break;
        case JNEG:
          mc_jneg (operand);
          return JNEG;
          break;
        case JZ:
          mc_jz (operand);
          return JZ;
          break;
        case HALT:
          mc_halt ();
          sc_halt = true;
          return HALT;
          break;
        case ADDC:
          mc_addc (operand);
          return ADDC;
          break;
        default:
          sc_regSet (FLAG_E, 1);
          return -2;
          break;
        }
    }
  return 0;
}

void
mc_oneTactPulse ()
{
  sc_halt = false;
  currMemCell = instruction_counter;
  mi_uiUpdate (sc_halt);
  sc_regSet (FLAG_T, 0);
  int cu_result = CU ();
  int value;
  sc_regGet (FLAG_T, &value);
  if (!(cu_result == 40 || cu_result == 41 || cu_result == 42
        || cu_result == -2))
    {
      sc_regSet (FLAG_E, 0);
      if ((instruction_counter >= 0 && instruction_counter <= 99) && !value)
        {
          if (instruction_counter != 99)
            {
              instruction_counter++;
            }
          else
            instruction_counter = 0;
        }
    }
  sc_regSet (FLAG_T, 1);
}
