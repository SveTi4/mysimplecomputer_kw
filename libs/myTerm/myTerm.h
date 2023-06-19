#pragma once
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define ACS_CKBOARD 'a'  // штриховка
#define ACS_ULCORNER 'l' // левый верхний угол
#define ACS_URCORNER 'k' // правый верхний угол
#define ACS_LRCORNER 'j' // правый нижний угол
#define ACS_LLCORNER 'm' // левый нижний угол
#define ACS_HLINE 'q'    // горизонтальная линия
#define ACS_VLINE 'x'    // вертикальная линия

enum colors
{
  DEFAULT,
  WHITE = 15,
  BLACK,
  RED = 196,
  ERROR,
  BLUE = 30,
  YELLOW = 220,
  GREEN = 34,
  PURPLE = 99,
  CYAN = 36,
  NONACTIVE = 0
};

int mt_clrscr (void);
int mt_gotoXY (int x, int y);
int mt_getScreenSize (int *rows, int *cols);
int mt_setFgColor (enum colors color);
int mt_setBgColor (enum colors color);
int mt_setDfColor (void);
int mt_printSymbol (char symbol);
