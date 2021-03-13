#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_Timers.h"

#include "Init.h"


#define clrScrn() 	puts("\x1b[2J")

int main (void)
{
  ES_Return_t ErrorType;
    
	// Your hardware initialization function calls go here
	// Set the clock to run at 40MhZ using the PLL and 16MHz external crystal
  SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  // Initialize the terminal for puts/printf debugging
  TERMIO_Init();
	clrScrn();
   
	// When doing testing, it is useful to announce just which program is running.
	puts("\rStarting KART prgram for ME218B Team10 \r");
	printf("%s %s\n",__TIME__, __DATE__);
	printf("\n\r\n");
	
	// Initialize all pins and peripherals
	InitAll();

	// Now initialize the Events and Services Framework and start it running
  ErrorType = ES_Initialize(ES_Timer_RATE_10mS);
  if ( ErrorType == Success ) 
	{
    ErrorType = ES_Run();
  }
	
	
	//if we got to here, there was an error
  switch (ErrorType){
    case FailedPointer:
      puts("Failed on NULL pointer");
      break;
    case FailedInit:
      puts("Failed Initialization");
      break;
    default:
      puts("Other Failure");
      break;
  }
	
	// hang after reporting error
  for(;;)   
    ;
}
