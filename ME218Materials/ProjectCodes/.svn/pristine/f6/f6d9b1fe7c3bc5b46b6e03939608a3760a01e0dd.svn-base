#include "DCMotorService.h"
#include "Init.h"
#include "IntSample.h"
#include "MotorPWM.h"

#include <stdio.h>
#include <termio.h>


void Rotate( bool Dir, uint8_t RPM)
{
	printf("\r\nROTATE!!");
	EnableIntSample(true);
	SetLMotorDesRPM(RPM);
	SetRMotorDesRPM(RPM);
	
	HWREG(MOTOR_DATA) &= ~( LEFT_MOTOR_BRAKE_PIN | RIGHT_MOTOR_BRAKE_PIN );
	
	if (Dir)
	{
		HWREG(MOTOR_DATA) |= LEFT_MOTOR_DIR_PIN;
		HWREG(MOTOR_DATA) &= ~RIGHT_MOTOR_DIR_PIN;
	}
	else
	{
		HWREG(MOTOR_DATA) |= RIGHT_MOTOR_DIR_PIN;
		HWREG(MOTOR_DATA) &= ~LEFT_MOTOR_DIR_PIN;
	}
}

void Move( bool Dir, uint8_t RPM)
{
	printf("\r\nMOVE!!");
	EnableIntSample(true);

	SetLMotorDesRPM(RPM);
	SetRMotorDesRPM(RPM);
	
	HWREG(MOTOR_DATA) &= ~( LEFT_MOTOR_BRAKE_PIN | RIGHT_MOTOR_BRAKE_PIN );
	
	if (Dir)
	{
		HWREG(MOTOR_DATA) |= LEFT_MOTOR_DIR_PIN;
		HWREG(MOTOR_DATA) |= RIGHT_MOTOR_DIR_PIN;
	}
	else
	{
		HWREG(MOTOR_DATA) &= ~RIGHT_MOTOR_DIR_PIN;
		HWREG(MOTOR_DATA) &= ~LEFT_MOTOR_DIR_PIN;
	}
}

void Brake()
{
	printf("\r\nBRAKE!!");
	HWREG(MOTOR_DATA) |= ( LEFT_MOTOR_BRAKE_PIN | RIGHT_MOTOR_BRAKE_PIN );
	
	
	// Need to set PWM to HIGH for successful brake
	EnableIntSample(false);
	SetMotorPWMDuty(100, LeftMotor);
	SetMotorPWMDuty(100, RightMotor);
}