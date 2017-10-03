#ifndef rit_h_
#define rit_h_
 
#include "LPC17xx.h"
#include <RTL.h>
extern OS_SEM mutex;
void rit_init(void);
void RIT_IRQHandler(void);
#endif
