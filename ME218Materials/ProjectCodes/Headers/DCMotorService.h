#ifndef DCMotorService_H
#define DCMotorService_H

#include <stdint.h>
#include <stdbool.h>

// Interface for controlling the locomotion motors
void Rotate( bool Dir, uint8_t RPM);
void Move( bool Dir, uint8_t RPM);
void Brake( void );

#endif
