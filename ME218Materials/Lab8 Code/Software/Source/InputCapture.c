
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
#include "PWM.h"


// 40,000 ticks per mS assumes a 40Mhz clock
#define TicksPerMS 40000

static uint32_t Period;
static uint32_t LastCapture;

/***************************************************************************
Beacon Sensing: Using WTimer0 Timer A to capture input on PC4
***************************************************************************/

void InitInputCapture( void ){

  // start by enabling the clock to the timer (Wide Timer 0)
  HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R0;
	// enable the clock to Port C
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R2;

  
  // make sure that timer (Timer A) is disabled before configuring
  HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
  
	// set it up in 32bit wide (individual, not concatenated) mode
	// the constant name derives from the 16/32 bit timer, but this is a 32/64
	// bit timer so we are setting the 32bit mode
  HWREG(WTIMER0_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;

	// we want to use the full 32 bit count, so initialize the Interval Load
	// register to 0xffff.ffff (its default value :-)
  HWREG(WTIMER0_BASE+TIMER_O_TAILR) = 0xffffffff;

	// we don't want any prescaler (it is unnecessary with a 32 bit count)
  HWREG(WTIMER0_BASE+TIMER_O_TAPR) = 0;

	// set up timer A, B in capture mode 
	// for edge time and up-counting
  HWREG(WTIMER0_BASE+TIMER_O_TAMR) = 
				(HWREG(WTIMER0_BASE+TIMER_O_TAMR) & ~TIMER_TAMR_TAAMS) | 
        (TIMER_TAMR_TACDIR | TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP);

	// To set the event to rising edge, we need to modify the TAEVENT bits 
	// in GPTMCTL. Rising edge = 00, so we clear the TAEVENT bits
  HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEVENT_M;


	// Now Set up the port to do the capture (clock was enabled earlier)
	// start by setting the alternate function for Port C bit 4 (WT0CCP0) and bit 5 (WT0CCP1)
	HWREG(GPIO_PORTC_BASE+GPIO_O_AFSEL) |= BIT4HI;

	// map bit 4's alternate function to WT0CCP0
	// map bit 5's alternate function to WT0CCP1
	HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) = 
    (HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) & 0xff00ffff) + (7<<(4*4));

	// Enable pin on Port C for digital I/O
	HWREG(GPIO_PORTC_BASE+GPIO_O_DEN) |= BIT4HI;
	
	// make pin 4 on Port C into an input
	HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) &= ~BIT4HI;

	// enable the Timer A in Wide Timer 0 interrupt in the NVIC
	// it is interrupt number 94 so appears in EN2 at bit 30
  HWREG(NVIC_EN2) |= BIT30HI;

	// make sure interrupts are enabled globally
  __enable_irq();


	// now kick the timer off by enabling it and enabling the timer to
	// stall while stopped by the debugger
  HWREG(WTIMER0_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL);

}

void InputCaptureResponseBeacon( void ){
	uint32_t ThisCapture;
	// start by clearing the source of the interrupt, the input capture event
  HWREG(WTIMER0_BASE+TIMER_O_ICR) = TIMER_ICR_CAECINT;
		
	// now grab the captured value and calculate the period
  ThisCapture = HWREG(WTIMER0_BASE+TIMER_O_TAR);
  Period = ThisCapture - LastCapture;
    
	// update LastCapture to prepare for the next edge  
  LastCapture = ThisCapture;
}

void EnableInputCapture(bool isEnabled, bool isBeacon)
{
	if (isBeacon)
	{
		if (isEnabled)
		{
			HWREG(WTIMER0_BASE+TIMER_O_IMR) |= TIMER_IMR_CAEIM;
		}
		else
		{
			HWREG(WTIMER0_BASE+TIMER_O_IMR) &= ~TIMER_IMR_CAEIM;
		}
	}
}

uint32_t GetPeriod( void ){
  return Period;
}
