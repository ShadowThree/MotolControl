#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern __IO uint16_t timeToEnd;

void TIM2_Int_Init(void);
void TIM3_Int_Init(void);
void TIM5_PWM_Init(u16 arr,u16 psc);
#endif
