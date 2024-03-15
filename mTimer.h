#ifndef M_TIMER
#define M_TIMER

// Timer callback function pointer type
typedef void(*TimerCallback)(void);

// Timer struct
struct mTimer{
  long unsigned int delayMs;
  long unsigned int nextInvoke;
  TimerCallback timerCallback;
};

/**
* Starts the timer if it is not already running.
* Default interval value can be overridden if int bigger that 0 is passed as second parameter.
*/
void timerStart(struct mTimer*, unsigned int);

/**
* Checks if timer has finished and calls timerCallback if it is. Sets nextinvoke to 0 if finished.
*/
void timerRun(struct mTimer*);

/**
* Forcefully stop timer sets nextInvoke back to 0.
*/
void timerForceStop(struct mTimer*);

#endif