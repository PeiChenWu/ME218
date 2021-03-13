#ifndef _IntSample_H_
#define _IntSample_H_

#include <stdint.h>
#include <stdbool.h>

void InitIntSample( void );
void EnableIntSample( bool enabled);
void ISR_PeriodicControl( void );

// Interface
void SetLMotorDesRPM (uint32_t RPM);

void SetRMotorDesRPM (uint32_t RPM);

#endif
