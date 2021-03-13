#ifndef INPUTCAPTURE_H
#define INPUTCAPTURE_H

#include <stdint.h>

void InitInputCapture( void );

// Interrupt Service Routines
void ISR_LeftEncoder( void );
void ISR_RightEncoder( void );
void ISR_Beacon( void );

uint32_t GetBeaconPeriod( void );
uint32_t GetLMotorPeriod( void );
uint32_t GetRMotorPeriod( void );

void SetLEncoderCount (uint32_t value);
uint32_t GetLEncoderCount ( void );

void SetREncoderCount (uint32_t value);
uint32_t GetREncoderCount ( void );

#endif
