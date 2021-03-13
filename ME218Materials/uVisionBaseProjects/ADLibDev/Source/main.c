#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "ES_Port.h"
#include "termio.h"
#include "ADCSWTrigger.h"

#define clrScrn() 	printf("\x1b[2J")


int main(void)
{  
  uint16_t StopTime;
  uint32_t Result;
  
  // initialize the timer sub-system and console I/O
  _HW_Timer_Init(ES_Timer_RATE_1mS);
	TERMIO_Init();
	clrScrn();

	// When doing testing, it is useful to announce just which program
	// is running.
	puts("\rStarting A/D Test \r");
	printf("%s %s\n",__TIME__, __DATE__);
	printf("\n\r\n");

	// Your hardware initialization function calls go here
  ADC0_InitSWTriggerSeq3(9);
  puts("\rA/D Initialized \r");
  
  // your program goes here
  while(1){
    StopTime = ES_Timer_GetTime() + 250;
    while(ES_Timer_GetTime() != StopTime){}
    Result = ADC0_InSeq3();
    printf("                  \r");    
    printf("A/D Result = %d\r",Result);
    
  }
}

