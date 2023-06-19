#pragma once
#include <fcntl.h>
#include <myTerm.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern int big_chars[][2];
extern int font[36];

int bc_printA (char str);
int bc_box (int x1, int y1, int x2, int y2);
int bc_printBigChar (int *big, int x, int y, enum colors fg, enum colors bg);
int bc_setBigCharPos (int *big, int x, int y, int value);
int bc_getBigCharPos (int *big, int x, int y, int *value);
int bc_bigCharWrite (int fd, int *big, int count);
int bc_bigCharRead (int fd, int *big, int need_count, int *count);
void bc_fontInit (char *filename);
