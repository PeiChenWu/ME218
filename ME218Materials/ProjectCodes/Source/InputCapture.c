/*****************************************************************************************************************
This module consists of set-up codes for using Input Capture function of the TIVA
and provides interfaces for results

Left  Motor Encoder: WT1CCP0 - PC6
Right Motor Encoder: WT1CCP1 - PC7

Beacon Sensing: WT0CCP0 - PC4
*****************************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "bitdefs.h"

#include "InputCapture.h"

// 40,000 ticks per mS assumes a 40Mhz clock
#define TicksPerMS 40000

static uint32_t LMotorPeriod;
static uint32_t RMotorPeriod;
static uint32_t BeaconPeriod;

static uint32_t LMotorLastCapture;
static uint32_t RMotorLastCapture;
static uint32_t BeaconLastCapture;

volatile static uint32_t LEncoderCount;
volatile static uint32_t REncoderCount;


// we will use Timer A in Wide Timer 0 to capture the input

void InitInputCapture( void ){

  // start by enabling the clock to the timers (Wide Timer 0 & Wide Timer 1)
  HWREG(SYSCTL_RCGCWTIMER) |= (SYSCTL_RCGCWTIMER_R0 | SYSCTL_RCGCWTIMER_R1);
  
  // make sure that timers are disabled before configuring
  HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEN;
	HWREG(WTIMER1_BASE+TIMER_O_CTL) &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
  
	// set it up in 32bit wide (individual, not concatenated) mode
	// the constant name derives from the 16/32 bit timer, but this is a 32/64 
	// bit timer so we are setting the 32bit mode
  HWREG(WTIMER0_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;
	HWREG(WTIMER1_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;

	// initialize the Interval Load register to 0xffffffff
	HWREG(WTIMER0_BASE+TIMER_O_TAILR) = 0xffffffff;
	HWREG(WTIMER1_BASE+TIMER_O_TAILR) = 0xffffffff;
	HWREG(WTIMER1_BASE+TIMER_O_TBILR) = 0xffffffff;

	// set up capture mode (TAMR=3, TAAMS = 0) for edge time (TACMR = 1) and up-counting (TACDIR = 1)
  HWREG(WTIMER0_BASE+TIMER_O_TAMR) = (HWREG(WTIMER0_BASE+TIMER_O_TAMR) & ~TIMER_TAMR_TAAMS) | 
																		 (TIMER_TAMR_TACDIR | TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP);

  HWREG(WTIMER1_BASE+TIMER_O_TAMR) = (HWREG(WTIMER0_BASE+TIMER_O_TAMR) & ~TIMER_TAMR_TAAMS) | 
																		 (TIMER_TAMR_TACDIR | TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP);
																		 
  HWREG(WTIMER1_BASE+TIMER_O_TBMR) = (HWREG(WTIMER0_BASE+TIMER_O_TBMR) & ~TIMER_TBMR_TBAMS) | 
																		 (TIMER_TBMR_TBCDIR | TIMER_TBMR_TBCMR | TIMER_TBMR_TBMR_CAP);

	// Set the event to rising edge, GPTMCTL = 00
  HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEVENT_M;
	HWREG(WTIMER1_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEVENT_M;
	HWREG(WTIMER1_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TBEVENT_M;

//	// Enable a local capture interrupt (Enable later)
//  HWREG(WTIMER0_BASE+TIMER_O_IMR) |= TIMER_IMR_CAEIM;
	HWREG(WTIMER1_BASE+TIMER_O_IMR) |= TIMER_IMR_CAEIM;
	HWREG(WTIMER1_BASE+TIMER_O_IMR) |= TIMER_IMR_CBEIM;

	// Enable interrupt in NVIC
  HWREG(NVIC_EN2) |= BIT30HI;		// WT0CCP0 - 94
	HWREG(NVIC_EN3) |= BIT0HI;		// WT1CCP0 - 96
	HWREG(NVIC_EN3) |= BIT1HI;		// WT1CCP1 - 97  

	// make sure interrupts are enabled globally
  __enable_irq();

	// now kick the timer off by enabling it and enabling the timer to
	// stall while stopped by the debugger
  HWREG(WTIMER0_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL);
	HWREG(WTIMER1_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL);
	HWREG(WTIMER1_BASE+TIMER_O_CTL) |= (TIMER_CTL_TBEN | TIMER_CTL_TBSTALL);
	
	LEncoderCount = 0;
	REncoderCount = 0;
}

void ISR_Beacon( void ){
  uint32_t ThisCapture;
	
	// start by clearing the source of the interrupt
  HWREG(WTIMER0_BASE+TIMER_O_ICR) = TIMER_ICR_CAECINT;
		
	// Grab the captured value and calculate the period
  ThisCapture = HWREG(WTIMER0_BASE+TIMER_O_TAR);
  BeaconPeriod = ThisCapture - BeaconLastCapture;
    
	// update LastCapture to prepare for the next edge  
  BeaconLastCapture = ThisCapture;
}

void ISR_LeftEncoder( void ){
  uint32_t ThisCapture;
	
	// start by clearing the source of the interrupt
  HWREG(WTIMER1_BASE+TIMER_O_ICR) = TIMER_ICR_CAECINT;
	
	LEncoderCount ++;
		
	// Grab the captured value and calculate the period
  ThisCapture = HWREG(WTIMER1_BASE+TIMER_O_TAR);
  LMotorPeriod = ThisCapture - LMotorLastCapture;
    
	// update LastCapture to prepare for the next edge  
  LMotorLastCapture = ThisCapture;
}

void ISR_RightEncoder( void ){
  uint32_t ThisCapture;
	
	// start by clearing the source of the interrupt
  HWREG(WTIMER1_BASE+TIMER_O_ICR) = TIMER_ICR_CBECINT;
	
	REncoderCount ++;
		
	// Grab the captured value and calculate the period
  ThisCapture = HWREG(WTIMER1_BASE+TIMER_O_TBR);
  RMotorPeriod = ThisCapture - RMotorLastCapture;
    
	// update LastCapture to prepare for the next edge  
  RMotorLastCapture = ThisCapture;
}

uint32_t GetBeaconPeriod( void )
{
	return BeaconPeriod;
}

uint32_t GetLMotorPeriod( void )
{
	return LMotorPeriod;
}

uint32_t GetRMotorPeriod( void )
{
	return RMotorPeriod;
}


void SetLEncoderCount (uint32_t value)
{
	LEncoderCount = value;
}

uint32_t GetLEncoderCount ( void )
{
	return LEncoderCount;
}

void SetREncoderCount (uint32_t value)
{
	REncoderCount = value;
}

uint32_t GetREncoderCount ( void )
{
	return REncoderCount;
}
