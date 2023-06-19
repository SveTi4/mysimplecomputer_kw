#pragma once
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#define CPU_CMDS_COUNT 38 // Количество команд
#define RAM_SIZE 100      // Размер оперативной памяти
#define REG_COUNT 5       // Количество регистров

#define FLAG_P 1 // Флаг переполнения при выполнении операции
#define FLAG_0 2 // Флаг ошибки деления на ноль
#define FLAG_M 3 // Флаг ошибки выхода за границы памяти
#define FLAG_T 4 // Флаг игнорирования тактовых импульсов
#define FLAG_E 5 // Флаг неверной команды

extern bool sc_halt;
extern int instruction_counter;
extern int accumulator;
extern short currMemCell;

int sc_memoryInit ();
int sc_memorySet (int address, int value);
int sc_memoryGet (int address, int *value);
int sc_memorySave (char *filename);
int sc_memoryLoad (char *filename);
int sc_regInit (void);
int sc_regSet (int registr, int value);
int sc_regGet (int registr, int *value);
int sc_commandEncode (int command, int operand, int *value);
int sc_commandDecode (int value, int *command, int *operand);
void sc_restart ();