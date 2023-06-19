#pragma once
#include "../../project/commands.h"
#include <myALU.h>
#include <myInterface.h>
#include <myReadKey.h>
#include <mySimpleComputer.h>
#include <myTerm.h>
#include <string.h>

int mc_read (int operand);
int mc_write (int operand);
int mc_load (int operand);
int mc_store (int operand);
int mc_jump (int operand);
int mc_jneg (int operand);
int mc_jz (int operand);
int mc_halt ();
int mc_loglc (int operand);
int CU ();
void mc_oneTactPulse ();