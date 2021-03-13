#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "driverlib/sysctl.h"

#include "ES_Port.h"
#include "termio.h"
#include "PWMTiva.h"

#define clrScrn() 	printf("\x1b[2J")


int main(void)
{  
		// Set the clock to run at 40MhZ using the PLL and 16MHz external crystal
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
			| SYSCTL_XTAL_16MHZ);
  // initialize the timer sub-system and console I/O
  _HW_Timer_Init(ES_Timer_RATE_1mS);
	TERMIO_Init();
	clrScrn();

	// When doing testing, it is useful to announce just which program
	// is running.
	puts("\rStarting PWMTest \r");
	printf("%s %s\n",__TIME__, __DATE__);
	printf("\n\r\n");

	// Your hardware initialization function calls go here
  PWM_TIVA_Init();
  PWM_TIVA_SetFreq(50, 0);
  PWM_TIVA_SetFreq(400, 1);
  
  PWM_TIVA_SetPulseWidth(1250,0);
  PWM_TIVA_SetDuty( 50, 1);
  PWM_TIVA_SetDuty( 25, 2);
  PWM_TIVA_SetDuty( 50, 3);
  
  // your program goes here
  putchar('A');
  getchar();
  putchar('l');
  getchar();
  putchar('l');
  getchar();
  putchar(' ');
  getchar();
  putchar('i');
  getchar();
  putchar('s');
  getchar();
  putchar(' ');
  getchar();
  putchar('g');
  getchar();
  putchar('o');
  getchar();
  putchar('o');
  getchar();
  putchar('d');
  getchar();

  
	// if you fall off the end of your code, then hang around here
	for(;;)
	  ;

}
