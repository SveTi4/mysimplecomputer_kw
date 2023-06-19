#include <myInterface.h>
#include <mySimpleComputer.h>
#include <myTerm.h>
int
main ()
{
  int value = 0;
  bc_fontInit ("mybch.font");
  sc_memoryInit ();
  printf ("\n>> инициализирована память\n");
  for (int i = 0; i < RAM_SIZE; i++)
    sc_memorySet (i, i + 1);
  for (int i = 0; i < RAM_SIZE; i++)
    {
      sc_memoryGet (i, &value);
      printf ("RAM[%d] = %d\n", i, value);
    }
  sc_memoryLoad ("memory.bin");
  printf ("\n>> чтение файла\n");
  for (int i = 0; i < RAM_SIZE; i++)
    {
      sc_memoryGet (i, &value);
      printf ("RAM[%d] = %d\n", i, value);
    }
  sc_regInit ();
  sc_regSet (FLAG_T, 1);
  printf ("\n>> поднят флаг для игнорирования тактовых импульсов\n");
  sc_regGet (FLAG_P, &value);
  printf ("переполнение при выполнении операции [FLAG_P] = %d\n", value);
  sc_regGet (FLAG_0, &value);
  printf ("ошибка деления на 0 [FLAG_0] = %d\n", value);
  sc_regGet (FLAG_M, &value);
  printf ("ошибка выхода за границы памяти [FLAG_M] = %d\n", value);
  sc_regGet (FLAG_T, &value);
  printf ("игнорирование тактовых импульсов [FLAG_T] = %d\n", value);
  sc_regGet (FLAG_E, &value);
  printf ("указана неверная команда [FLAG_E] = %d\n", value);
  printf ("\n>> кодирование\n");
  int encode, oper, com = 0;
  sc_commandEncode (0x33, 0x59, &encode);
  printf ("закодированная команда: %x\n", encode);
  sc_commandDecode (encode, &com, &oper);
  printf ("команда: %x\nоперанд: %x\n\n", com, oper);

  int counter = 0x35;
  sc_memorySet (counter + 5, 0x4a81);
  sc_memorySet (counter, 0x0);
  sc_memorySet (counter - 2, 0x2be4);
  getchar_unlocked ();
  mi_uiInit ();
  mi_uiUpdate ();
  return 0;
}