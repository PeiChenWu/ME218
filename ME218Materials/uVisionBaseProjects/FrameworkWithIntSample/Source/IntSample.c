
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "bitdefs.h"

#include "IntSample.h"

// 40,000 ticks per mS assumes a 40Mhz clock
#define TicksPerMS 40000

static uint32_t TimeoutCount;

// we will use Timer A in Wide Timer 0 to generate the interrupt

void InitPeriodicInt( void ){
  volatile uint32_t Dummy; // use volatile to avoid over-optimization
  
  // start by enabling the clock to the timer (Wide Timer 0)
  HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R0;
	// kill a few cycles to let the clock get going
	Dummy = HWREG(SYSCTL_RCGCGPIO);
  
  // make sure that timer (Timer A) is disabled before configuring
  HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEN;
  
// set it up in 32bit wide (individual, not concatenated) mode
// the constant name derives from the 16/32 bit timer, but this is a 32/64
// bit timer so we are setting the 32bit mode
  HWREG(WTIMER0_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;
  
// set up timer A in periodic mode so that it repeats the time-outs
  HWREG(WTIMER0_BASE+TIMER_O_TAMR) = 
     (HWREG(WTIMER0_BASE+TIMER_O_TAMR)& ~TIMER_TAMR_TAMR_M)| 
     TIMER_TAMR_TAMR_PERIOD;

// set timeout to 100mS
  HWREG(WTIMER0_BASE+TIMER_O_TAILR) = TicksPerMS * 100;

// enable a local timeout interrupt
  HWREG(WTIMER0_BASE+TIMER_O_IMR) |= TIMER_IMR_TATOIM;

// enable the Timer A in Wide Timer 0 interrupt in the NVIC
// it is interrupt number 94 so apppears in EN2 at bit 30
  HWREG(NVIC_EN2) = BIT30HI;

// make sure interrupts are enabled globally
  __enable_irq();
  
// now kick the timer off by enabling it and enabling the timer to
// stall while stopped by the debugger
  HWREG(WTIMER0_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL);

}

void PeriodicIntResponse( void ){
// start by clearing the source of the interrupt
    HWREG(WTIMER0_BASE+TIMER_O_ICR) = TIMER_ICR_TATOCINT;
// increment our counter so that we can tell this is running  
    ++TimeoutCount;
}

uint32_t GetTimeoutCount( void ){
  return TimeoutCount;
}
