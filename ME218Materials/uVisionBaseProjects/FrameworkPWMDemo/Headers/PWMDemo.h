#ifndef PWMDEMO_H
#define PWMDEMO_H

#include <stdint.h>

void InitPWMDemo( void );
void Set100_0DC(void);
void SetPWMDuty( uint8_t NewDuty );
void SetPWMPeriod( uint32_t NewPeriod );
void SetPWMPulseWidth (uint32_t NewWidth );

#endif
