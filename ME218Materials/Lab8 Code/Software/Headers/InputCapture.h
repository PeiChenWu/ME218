#ifndef INPUTCAPTURE_H
#define INPUTCAPTURE_H

#include <stdint.h>
#include <stdbool.h>

void InitInputCapture( void );
void InputCaptureResponseBeacon( void );
void EnableInputCapture(bool isEnabled, bool isBeacon);
uint32_t GetPeriod( void );

#endif