#include "mySimpleComputer.h"

int memory[RAM_SIZE]; // Модель оперативной памяти
int sc_regFlags;      // Регистр флагов
bool sc_halt;
int instruction_counter;
int accumulator;
short currMemCell;

int
sc_memoryInit ()
{
  for (int i = 0; i < RAM_SIZE; i++)
    {
      memory[i] = 0;
    }
  return 0;
}

int
sc_memorySet (int address, int value)
{
  if (address < 0 || address >= RAM_SIZE)
    {
      sc_regSet (FLAG_M, 1);
      return FLAG_M;
    }
  memory[address] = value;
  return 0;
}

int
sc_memoryGet (int address, int *value)
{
  if (address < 0 || address >= RAM_SIZE)
    {
      sc_regSet (FLAG_M, 1);
      return FLAG_M;
    }
  *value = memory[address];
  return 0;
}

int
sc_memorySave (char *filename)
{
  FILE *fp;
  fp = fopen (filename, "wb");
  if (fp == NULL)
    {
      return -1;
    }
  fwrite (memory, RAM_SIZE, sizeof (RAM_SIZE), fp);
  fclose (fp);
  return 0;
}

int
sc_memoryLoad (char *filename)
{
  FILE *fp;
  fp = fopen (filename, "rb");
  if (fp == NULL)
    {
      return -1;
    }
  fread (memory, RAM_SIZE, sizeof (RAM_SIZE), fp);
  fclose (fp);
  return 0;
}

int
sc_regInit (void)
{
  sc_halt = false;
  sc_regFlags = 0;
  return 0;
}

int
sc_regSet (int registr, int value)
{
  if (registr > REG_COUNT || registr <= 0)
    {
      return -2;
    }
  if (value < 0 || value > 1)
    {
      return -3;
    }
  if (value == 1)
    {
      sc_regFlags |= (1 << (registr - 1));
    }
  else
    {
      sc_regFlags &= ~(1 << (registr - 1));
    }
  return 0;
}

int
sc_regGet (int registr, int *value)
{
  if (registr > REG_COUNT || registr <= 0)
    {
      return -2;
    }
  *value = (sc_regFlags >> (registr - 1)) & 0x1;
  return 0;
}

int
sc_commandEncode (int command, int operand, int *value)
{
  if (operand < 0 || operand > 127)
    return -5;
  *value = operand + (command << 7);
  return 0;
}

int
sc_commandDecode (int value, int *command, int *operand)
{
  if (value >> 14)
    {
      sc_regSet (FLAG_E, 1);
      return FLAG_E;
    }
  *command = (value >> 7) & 0x7F;
  *operand = value & 0x7F;
  return 0;
}

void
sc_restart ()
{
  sc_halt = false;
  raise (SIGUSR1);
  accumulator = 0;
  instruction_counter = 0;
  currMemCell = 0;
  sc_memoryInit ();
  sc_regSet (FLAG_P, 0);
  sc_regSet (FLAG_0, 0);
  sc_regSet (FLAG_M, 0);
  sc_regSet (FLAG_T, 1);
  sc_regSet (FLAG_E, 0);
}