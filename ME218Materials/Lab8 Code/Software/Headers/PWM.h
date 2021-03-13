#ifndef PWM_H
#define PWM_H

#define LeftMotor 6
#define RightMotor 7

#include <stdint.h>
#include <stdbool.h>

void InitPWM(void);
void setPWMDuty(uint8_t pwmDuty, uint8_t pwmChannel);		// unit: % (0 - 100), LeftMotor or RightMotor
void setPWMPolarity(bool invertEn, uint8_t pwmChannel);// invert PWM

#endif