#include <myALU.h>
#include <myCU.h>
#include <myInterface.h>
#include <mySimpleComputer.h>

void ms_setTimer (struct itimerval *nval, struct itimerval *oval);
void ms_setSignals ();
void ms_timerHandler (int sig);
void ms_userSignal (int sig);