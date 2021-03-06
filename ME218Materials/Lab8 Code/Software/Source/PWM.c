/*******************************************************************
This is the PWM module for controlling the speed of the two motors
that drives the droid

Use M0PWM6 on Pin PD0 controlled by Module 0 PWM generator 3 CmpA
Use M0PWM7 on Pin PD1 controlled by Module 0 PWM generator 3 CmpB
*******************************************************************/

#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "bitdefs.h"

#include "PWM.h"

// 40,000 ticks per mS assumes a 40Mhz clock, we will use SysClk/32 for PWM
#define PWMTicksPerMS 40000/32
#define PWMFrequency 400
#define BitsPerNibble 4


void InitPWM(void)
{
	volatile uint32_t Dummy; // use volatile to avoid over-optimization
  
  HWREG(SYSCTL_RCGCPWM) |= SYSCTL_RCGCPWM_R0;		// enable the clock to the PWM Module (PWM0)
	
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R3;	// enable the clock to Port D  

	// Select the PWM clock as System Clock/32
  HWREG(SYSCTL_RCC) = (HWREG(SYSCTL_RCC) & ~SYSCTL_RCC_PWMDIV_M) |
    (SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_32);
  
	// make sure that the PWM module clock has gotten going
	while ((HWREG(SYSCTL_PRPWM) & SYSCTL_PRPWM_R0) != SYSCTL_PRPWM_R0)
    ;

	// disable the PWM while initializing
  HWREG( PWM0_BASE+PWM_O_3_CTL ) = 0;
	
	// program generator A to go to 0 at rising compare A, 1 on falling compare A  
  HWREG( PWM0_BASE+PWM_O_3_GENA) = 
    (PWM_3_GENA_ACTCMPAU_ZERO | PWM_3_GENA_ACTCMPAD_ONE );

	// program generator B to go to 0 at rising compare B, 1 on falling compare B  
  HWREG( PWM0_BASE+PWM_O_3_GENB) = 
    (PWM_3_GENA_ACTCMPBU_ZERO | PWM_3_GENA_ACTCMPBD_ONE );
	
	// Set the PWM period. Since we are counting both up & down, we initialize
	// the load register to 1/2 the desired total period
  HWREG( PWM0_BASE+PWM_O_3_LOAD) = (1000 * PWMTicksPerMS / PWMFrequency)>>1;
  
	// Set the initial Duty cycle to 50% by programming the compare value
	// to 1/2 the period to count up (or down) 
  // HWREG( PWM0_BASE+PWM_O_3_CMPA) = (1000 * PWMTicksPerMS / PWMFrequency)>>2;
	// HWREG( PWM0_BASE+PWM_O_3_CMPB) = (1000 * PWMTicksPerMS / PWMFrequency)>>2;

	// set changes to the PWM output Enables to be locally syncronized to a 
	// zero count
    HWREG(PWM0_BASE+PWM_O_ENUPD) =  (HWREG(PWM0_BASE+PWM_O_ENUPD) & 
      ~(PWM_ENUPD_ENUPD6_M | PWM_ENUPD_ENUPD7_M)) |
      (PWM_ENUPD_ENUPD6_LSYNC | PWM_ENUPD_ENUPD7_LSYNC);

	// enable the PWM outputs
  HWREG( PWM0_BASE+PWM_O_ENABLE) |= (PWM_ENABLE_PWM6EN | PWM_ENABLE_PWM7EN);

	// now configure the Port D pins to be PWM outputs
  HWREG(GPIO_PORTD_BASE+GPIO_O_AFSEL) |= (BIT0HI | BIT1HI);	// select alternate functions

	// now choose to map PWM to those pins
  HWREG(GPIO_PORTD_BASE+GPIO_O_PCTL) = 
    (HWREG(GPIO_PORTD_BASE+GPIO_O_PCTL) & 0xffffff00) + (4<<(0*BitsPerNibble)) + (4<<(1*BitsPerNibble));

	HWREG(GPIO_PORTD_BASE+GPIO_O_DEN) |= (BIT0HI | BIT1HI);
	HWREG(GPIO_PORTD_BASE+GPIO_O_DIR) |= (BIT0HI | BIT1HI);
	
	// Set PWMDuty to be 0 to make sure motor won't move at the very beginning
	setPWMDuty(50, LeftMotor);
	setPWMDuty(50, RightMotor);
  
	// set the up/down count mode and enable the PWM generator
  HWREG(PWM0_BASE+ PWM_O_3_CTL) |= (PWM_3_CTL_MODE | PWM_3_CTL_ENABLE);	
}

void setPWMDuty(uint8_t pwmDuty, uint8_t pwmChannel)
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
}

void setPWMPolarity(bool invertEn, uint8_t pwmChannel)
{
	if (pwmChannel == LeftMotor)
	{
		if (invertEn)
		{
			HWREG( PWM0_BASE + PWM_O_INVERT) |= PWM_INVERT_PWM6INV;
		}
		else
		{
			HWREG( PWM0_BASE + PWM_O_INVERT) &= ~PWM_INVERT_PWM6INV;
		}
	}
	else
	{
		if (invertEn)
		{
			HWREG( PWM0_BASE + PWM_O_INVERT) |= PWM_INVERT_PWM7INV;
		}
		else
		{
			HWREG( PWM0_BASE + PWM_O_INVERT) &= ~PWM_INVERT_PWM7INV;
		}
	}
}