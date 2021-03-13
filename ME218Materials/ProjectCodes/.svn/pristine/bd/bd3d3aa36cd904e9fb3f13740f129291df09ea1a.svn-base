#ifndef MOTORPWM_H
#define MOTORPWM_H

#define LeftMotor 6
#define RightMotor 7
#define PitchMotor 2
#define ServoMotor1 4
#define ServoMotor2 5

#include <stdint.h>
#include <stdbool.h>

void InitMotorPWM(void);
void SetMotorPWMDuty(uint8_t pwmDuty, uint8_t pwmChannel);		// unit: % (0 - 100), LeftMotor, RightMotor, or PitchMotor
void SetServoPWMPulseWidth(uint16_t pulseWidth, uint8_t pwmChannel);
#endif
