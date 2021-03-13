/*****************************************************************************************************************
This is the PWM module for controlling the speed of the two motors that drives the KART
and the pitching motor for shooting

Use M0PWM6 on Pin PD0 controlled by Module 0 PWM generator 3 CmpA
Use M0PWM7 on Pin PD1 controlled by Module 0 PWM generator 3 CmpB
Use M0PWM2 on Pin PB4 controlled by Module 0 PWM generator 1 CmpA
Use M0PWM4 on Pin PE4 controlled by Module 0 PWM generator 2 CmpA
Use M0PWM5 on Pin PE5 controlled by Module 0 PWM generator 2 CmpB
*****************************************************************************************************************/

#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "bitdefs.h"

#include "MotorPWM.h"

// 40,000 ticks per mS assumes a 40Mhz clock, we will use SysClk/32 for PWM
#define PWMTicksPerMS 40000/32
#define PWMFrequency 400
#define PWMFrequencyPitch 400
#define PWMFrequencyServo 50


void InitMotorPWM(void)
{	 
  HWREG(SYSCTL_RCGCPWM) |= SYSCTL_RCGCPWM_R0;		// enable the clock to the PWM Module (PWM0)

	// Select the PWM clock as System Clock/32
  HWREG(SYSCTL_RCC) = (HWREG(SYSCTL_RCC) & ~SYSCTL_RCC_PWMDIV_M) | (SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_32);
  
	// make sure that the PWM module clock has gotten going
	while ((HWREG(SYSCTL_PRPWM) & SYSCTL_PRPWM_R0) != SYSCTL_PRPWM_R0);

	// disable the PWM while initializing
  HWREG( PWM0_BASE+PWM_O_3_CTL ) = 0;
	HWREG( PWM0_BASE+PWM_O_1_CTL ) = 0;
	HWREG( PWM0_BASE+PWM_O_2_CTL ) = 0;
	
	
	// go to 0 at rising compare A/B, 1 on falling compare A/B  
  HWREG( PWM0_BASE+PWM_O_3_GENA) = (PWM_3_GENA_ACTCMPAU_ZERO | PWM_3_GENA_ACTCMPAD_ONE ); 
  HWREG( PWM0_BASE+PWM_O_3_GENB) = (PWM_3_GENA_ACTCMPBU_ZERO | PWM_3_GENA_ACTCMPBD_ONE );
	HWREG( PWM0_BASE+PWM_O_1_GENA) = (PWM_1_GENA_ACTCMPAU_ZERO | PWM_1_GENA_ACTCMPAD_ONE );
	HWREG( PWM0_BASE+PWM_O_2_GENA) = (PWM_2_GENA_ACTCMPAU_ZERO | PWM_2_GENA_ACTCMPAD_ONE ); 
  HWREG( PWM0_BASE+PWM_O_2_GENB) = (PWM_2_GENA_ACTCMPBU_ZERO | PWM_2_GENA_ACTCMPBD_ONE );
	
	
	// Set the PWM period. Since we are counting both up & down, we initialize
	// the load register to 1/2 the desired total period
  HWREG( PWM0_BASE+PWM_O_3_LOAD) = (1000 * PWMTicksPerMS / PWMFrequency)>>1;
	HWREG( PWM0_BASE+PWM_O_1_LOAD) = (1000 * PWMTicksPerMS / PWMFrequencyPitch)>>1;
	HWREG( PWM0_BASE+PWM_O_2_LOAD) = (1000 * PWMTicksPerMS / PWMFrequencyServo)>>1;

	// set changes to the PWM output Enables to be locally syncronized to a zero count
  HWREG(PWM0_BASE+PWM_O_ENUPD) =  (HWREG(PWM0_BASE+PWM_O_ENUPD) &
																	~(PWM_ENUPD_ENUPD6_M | PWM_ENUPD_ENUPD7_M | 
																		PWM_ENUPD_ENUPD2_M | PWM_ENUPD_ENUPD4_M | 
																		PWM_ENUPD_ENUPD5_M)) | 
																	(PWM_ENUPD_ENUPD6_LSYNC | PWM_ENUPD_ENUPD7_LSYNC | 
																	 PWM_ENUPD_ENUPD2_LSYNC | PWM_ENUPD_ENUPD4_LSYNC | 
																	 PWM_ENUPD_ENUPD5_LSYNC);

	// enable the PWM outputs
  HWREG( PWM0_BASE+PWM_O_ENABLE) |= (PWM_ENABLE_PWM6EN | PWM_ENABLE_PWM7EN | 
																		 PWM_ENABLE_PWM2EN | 
																		 PWM_ENABLE_PWM4EN | PWM_ENABLE_PWM5EN);

	// Set PWMDuty to be 0 to make sure motor won't move at the very beginning
	SetMotorPWMDuty(0, LeftMotor);
	SetMotorPWMDuty(0, RightMotor);
	SetMotorPWMDuty(0, PitchMotor);
  
	// set the up/down count mode and enable the PWM generator
  HWREG(PWM0_BASE+ PWM_O_3_CTL) |= (PWM_3_CTL_MODE | PWM_3_CTL_ENABLE);	
	HWREG(PWM0_BASE+ PWM_O_1_CTL) |= (PWM_1_CTL_MODE | PWM_1_CTL_ENABLE);	
	HWREG(PWM0_BASE+ PWM_O_2_CTL) |= (PWM_2_CTL_MODE | PWM_2_CTL_ENABLE);	
}

void SetMotorPWMDuty(uint8_t pwmDuty, uint8_t pwmChannel)
{
	if (pwmChannel == LeftMotor)
	{
		if (pwmDuty == 0)
		{
			HWREG( PWM0_BASE+PWM_O_3_CMPA) = 0;
			HWREG( PWM0_BASE+PWM_O_ENABLE) &= ~PWM_ENABLE_PWM6EN;
		}
	
		else if (pwmDuty > 99)
		{
			// Set the Duty cycle on B to 100% by programming the compare value
			// to the load value. Since the CmpBDn action (set to one) wins, we get 100%
			HWREG( PWM0_BASE+PWM_O_ENABLE) |= PWM_ENABLE_PWM6EN;
			HWREG( PWM0_BASE+PWM_O_3_CMPA) = HWREG( PWM0_BASE+PWM_O_3_LOAD);
		}
		else
		{
			HWREG( PWM0_BASE+PWM_O_ENABLE) |= PWM_ENABLE_PWM6EN;
			HWREG( PWM0_BASE+PWM_O_3_CMPA) = HWREG( PWM0_BASE+PWM_O_3_LOAD) * pwmDuty / 100;		
		}
	}
	else
	if (pwmChannel == RightMotor)
	{
		if (pwmDuty == 0)
		{
			HWREG( PWM0_BASE+PWM_O_3_CMPB) = 0;
			HWREG( PWM0_BASE+PWM_O_ENABLE) &= ~PWM_ENABLE_PWM7EN;
		}
	
		else if (pwmDuty > 99)
		{
			// Set the Duty cycle on B to 100% by programming the compare value
			// to the load value. Since the CmpBDn action (set to one) wins, we get 100%
			HWREG( PWM0_BASE+PWM_O_ENABLE) |= PWM_ENABLE_PWM7EN;
			HWREG( PWM0_BASE+PWM_O_3_CMPB) = HWREG( PWM0_BASE+PWM_O_3_LOAD);
		}
		else
		{
			HWREG( PWM0_BASE+PWM_O_ENABLE) |= PWM_ENABLE_PWM7EN;
			HWREG( PWM0_BASE+PWM_O_3_CMPB) = HWREG( PWM0_BASE+PWM_O_3_LOAD) * pwmDuty / 100;		
		}
	}
	else
	if (pwmChannel == PitchMotor)
	{
		if (pwmDuty == 0)
		{
			HWREG( PWM0_BASE+PWM_O_1_CMPB) = 0;
			HWREG( PWM0_BASE+PWM_O_ENABLE) &= ~PWM_ENABLE_PWM2EN;
		}
	
		else if (pwmDuty > 99)
		{
			// Set the Duty cycle on B to 100% by programming the compare value
			// to the load value. Since the CmpBDn action (set to one) wins, we get 100%
			HWREG( PWM0_BASE+PWM_O_ENABLE) |= PWM_ENABLE_PWM2EN;
			HWREG( PWM0_BASE+PWM_O_1_CMPB) = HWREG( PWM0_BASE+PWM_O_1_LOAD);
		}
		else
		{
			HWREG( PWM0_BASE+PWM_O_ENABLE) |= PWM_ENABLE_PWM2EN;
			HWREG( PWM0_BASE+PWM_O_1_CMPB) = HWREG( PWM0_BASE+PWM_O_1_LOAD) * pwmDuty / 100;		
		}
	}
}

void SetServoPWMPulseWidth(uint16_t pulseWidth, uint8_t pwmChannel)
{
	// Acceptable range of pulseWidth (1000 - 2000)(1ms - 2ms)
	if (pwmChannel == ServoMotor1)
	{
		HWREG( PWM0_BASE+PWM_O_2_CMPA) = (pulseWidth * 40000) << 1 ;
	}
	else
	if (pwmChannel == ServoMotor2)
	{
		HWREG( PWM0_BASE+PWM_O_2_CMPB) = (pulseWidth * 40000) << 1 ;
	}
}