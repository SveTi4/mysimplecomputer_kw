#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

enum keys
{
  ESC_KEY,
  L_KEY,
  S_KEY,
  R_KEY,
  T_KEY,
  I_KEY,
  F5_KEY,
  F6_KEY,
  UP_KEY,
  DOWN_KEY,
  LEFT_KEY,
  RIGHT_KEY,
  ENTER_KEY,
  NOTHING_KEY,
  INVALID_KEY,
};

extern struct termios save;

int rk_readKey (enum keys *key);
int rk_myTermSave ();
int rk_myTermRestore ();
int rk_myTermRegime (int regime, int vtime, int vmin, int echo, int sigit);
