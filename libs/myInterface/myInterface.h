#pragma once
#include <ctype.h>
#include <dirent.h>
#include <myBigChars.h>
#include <myReadKey.h>
#include <mySimpleComputer.h>
#include <myTerm.h>
#include <sys/stat.h>
#include <time.h>

#define ROWS 24
#define COLUMNS 80

#define MEMORY_X 1
#define MEMORY_Y 1
#define MEMORY_WIDTH 61
#define MEMORY_HEIGHT 12

#define ACC_X 62
#define ACC_Y 1
#define ACC_WIDTH 22
#define ACC_HEIGHT 3

#define COUNTER_X 62
#define COUNTER_Y 4
#define COUNTER_WIDTH 22
#define COUNTER_HEIGHT 3

#define OP_X 62
#define OP_Y 7
#define OP_WIDTH 22
#define OP_HEIGHT 3

#define FLAGS_X 62
#define FLAGS_Y 10
#define FLAGS_WIDTH 22
#define FLAGS_HEIGHT 3

#define BIG_CHARS_X 1
#define BIG_CHARS_Y 13
#define BIG_CHARS_WIDTH 52
#define BIG_CHARS_HEIGHT 10

#define KEYS_X 53
#define KEYS_Y 13
#define KEYS_WIDTH 31
#define KEYS_HEIGHT 10

int mi_displayMemoryValues ();
int mi_displayAccumulator ();
int mi_displayInstructionCounter ();
int mi_displayOperation ();
int mi_displayFlags ();
int mi_displayBoxes ();
int mi_displayTexts ();
int mi_displayBigChars ();
int mi_displayAccumulatorBigChars ();
int mi_counter ();
int mi_accum ();
int mi_uiInit ();
int mi_uiUpdate (bool halt);
int mi_uiSetValue ();
bool mi_checkCorrectInput (const char buffer[10]);
int mi_messageOutput (char *str, enum colors color);
int mi_clearBuffIn ();
int mi_hideCursor ();
int mi_showCursor ();
void mi_dirMenu ();
int mi_memorySave ();
int mi_currMemMove (enum keys direction);