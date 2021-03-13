/*****************************************************************************************************************
This module consists of set-up codes for using Wide Timer2 to execute control law on the motors

*****************************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "bitdefs.h"

#include "IntSample.h"
#include "InputCapture.h"
#include "MotorPWM.h"

// 40,000 ticks per mS assumes a 40Mhz clock
#define TicksPerMS 40000

static uint32_t LMotorCurRPM;
static uint32_t LMotorDesRPM;
static int32_t LMotorRPMError;
static int32_t LMotorSumError;

static uint32_t RMotorCurRPM;
static uint32_t RMotorDesRPM;
static int32_t RMotorRPMError;
static int32_t RMotorSumError;

static float LKp;
static float LKi;
static float RKp;
static float RKi;

static void GetLMotorCurRPM (void);
static void GetRMotorCurRPM (void);

// we will use Timer A in Wide Timer 0 to generate the interrupt

void InitIntSample( void ){
  volatile uint32_t Dummy; // use volatile to avoid over-optimization
  
  // start by enabling the clock to the timer (Wide Timer 2)
	
  HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R2;
	// kill a few cycles to let the clock get going
	Dummy = HWREG(SYSCTL_RCGCGPIO);
  
  // make sure that timer (Timer A) is disabled before configuring
  HWREG(WTIMER2_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEN;
  
	// set it up in 32bit wide
  HWREG(WTIMER2_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;
  
	// set up timer A in periodic mode so that it repeats the time-outs
  HWREG(WTIMER2_BASE+TIMER_O_TAMR) = (HWREG(WTIMER1_BASE+TIMER_O_TAMR)& ~TIMER_TAMR_TAMR_M)| 
																		 TIMER_TAMR_TAMR_PERIOD;

	// set timeout to 20mS
  HWREG(WTIMER2_BASE+TIMER_O_TAILR) = TicksPerMS * 20;

	// enable a local timeout interrupt
  HWREG(WTIMER2_BASE+TIMER_O_IMR) |= TIMER_IMR_TATOIM;

	// enable NVIC (98)
  HWREG(NVIC_EN3) = BIT2HI;

	// make sure interrupts are enabled globally
  __enable_irq();
  
	// now kick the timer off by enabling it and enabling the timer to
	// stall while stopped by the debugger
  HWREG(WTIMER2_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL);

	LMotorSumError = 0;
	RMotorSumError = 0;
	
	LMotorDesRPM = 0;
	RMotorDesRPM = 0;
	
	SetMotorPWMDuty(0, LeftMotor);
	SetMotorPWMDuty(0, RightMotor);
	
	LKp = 0.7;
	LKi = 0.2;
	
	RKp = 0.6;
	RKi = 0.2;
}

void ISR_PeriodicControl( void ){
	// start by clearing the source of the interrupt
  HWREG(WTIMER2_BASE+TIMER_O_ICR) = TIMER_ICR_TATOCINT;
	
	GetLMotorCurRPM();
	LMotorRPMError = LMotorDesRPM - LMotorCurRPM;
	LMotorSumError += LMotorRPMError;
	
	//printf("\r\nRPM Error: %d", LMotorRPMError);
	
	GetRMotorCurRPM();
	RMotorRPMError = RMotorDesRPM - RMotorCurRPM;
	RMotorSumError += RMotorRPMError;
	
	//printf("\r\nRPM Error: %d", RMotorRPMError);
	
	
	// Control law for left motor
	float LeftDC = LKp * (LMotorRPMError + LKi * LMotorSumError);
	if (LeftDC > 100)
	{
		LMotorSumError -= LMotorRPMError;
		LeftDC = 100;
	}
	if (LeftDC < 0)
	{
		LeftDC = 0;
		LMotorSumError -= LMotorRPMError;
	}
	//printf("\r\n Left Duty Cycle: %f", LeftDC);
	SetMotorPWMDuty(LeftDC, LeftMotor);
	
	// Control law for right motor
	float RightDC = RKp * (RMotorRPMError + RKi * RMotorSumError);
	if (RightDC > 100)
	{
		RMotorSumError -= RMotorRPMError;
		RightDC = 100;
	}
	if (RightDC < 0)
	{
		RightDC = 0;
		RMotorSumError -= RMotorRPMError;
	}
	
	//printf("\r\n Right Duty Cycle: %f", RightDC);
	SetMotorPWMDuty(RightDC, RightMotor);
}

void EnableIntSample( bool enabled)
{
	if (enabled)
		// enable a local timeout interrupt
		HWREG(WTIMER2_BASE+TIMER_O_IMR) |= TIMER_IMR_TATOIM;
	else
		HWREG(WTIMER2_BASE+TIMER_O_IMR) &= ~TIMER_IMR_TATOIM;
}

void SetLMotorDesRPM (uint32_t RPM)
{
	LMotorDesRPM = RPM;
}


void SetRMotorDesRPM (uint32_t RPM)
{
	RMotorDesRPM = RPM;
}


static void GetLMotorCurRPM (void)
{
	static uint32_t LastLMotorCount = 0;
	
	uint32_t LMotorPeriod = GetLMotorPeriod();
	uint32_t TimePerRevolution = 25 * LMotorPeriod;		// In microsecond
	LMotorCurRPM = 1000000000 / TimePerRevolution;
	
//	uint32_t ThisLMotorCount = GetLEncoderCount();
//	if (LastLMotorCount == ThisLMotorCount)
//	{
//		LMotorCurRPM = 0;
//	}
	
	if (TimePerRevolution > 1000000000 )
		LMotorCurRPM = 0;
	
	//LastLMotorCount = ThisLMotorCount;
	//printf("\r\nCurrent L RPM: %d", LMotorCurRPM);
}

static void GetRMotorCurRPM (void)
{
	static uint32_t LastRMotorCount = 0;
	
	uint32_t RMotorPeriod = GetRMotorPeriod();
	uint32_t TimePerRevolution = 25 * RMotorPeriod ;		// In nanosecond
	RMotorCurRPM = 1000000000 / TimePerRevolution;
	
//	uint32_t ThisRMotorCount = GetREncoderCount();
//	if (LastRMotorCount == ThisRMotorCount)
//	{
//		RMotorCurRPM = 0;
//	}
	
	if (TimePerRevolution > 1000000000 )
		RMotorCurRPM = 0;
	
	//LastRMotorCount = ThisRMotorCount;
	//printf("\r\nCurrent R RPM: %d", RMotorCurRPM);
}