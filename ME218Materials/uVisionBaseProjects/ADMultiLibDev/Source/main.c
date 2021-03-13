#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

#include "ES_Port.h"
#include "termio.h"
#include "ADMulti.h"

#define clrScrn() 	printf("\x1b[2J")

static uint32_t ADResults[4];

int main(void)
{  
  // initialize the timer sub-system and console I/O
  _HW_Timer_Init(ES_Timer_RATE_1mS);
	TERMIO_Init();
	clrScrn();

	// When doing testing, it is useful to announce just which program
	// is running.
	puts("\rStarting ADMulti Test \r");
	printf("%s %s\n",__TIME__, __DATE__);
	printf("\n\r\n");

	// Your hardware initialization function calls go here
  ADC_MultiInit(3);
  
  // your program goes here
  while (1){
    ADC_MultiRead(ADResults);
    printf(" Ch0 = %u, Ch1 = %u, Ch2 = %u, Ch3 = %u \n\r", ADResults[0], ADResults[1],
          ADResults[2], ADResults[3]);
    puts("Press any key to do the next conversion\r");
    getchar();
  }
  
}
