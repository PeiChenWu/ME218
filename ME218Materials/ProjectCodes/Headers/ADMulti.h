/***********************Instruction from Ed's email******************************
The library (ADMulti.c & ADMulti.h) provides for reading 1-4 channels from the A/D converter.

You initialize it with a call to ADC_MultiInit( HowMany) , where HowMany ranges 
from 1-4 and specifies the number of A/D channels desired.

You read the results with ADC_MultiRead( results) where results is an array with 
an element for each conversion result (yes, it needs to be declared as an array, 
even if it has only one element).

If you ask for 1 channel, use PE0 as the analog input,

If you ask for 2 channels, use PE0 & PE1 as the analog inputs,

If you ask for 3 channels, use PE0, PE1 & PE2 as the analog inputs,

If you ask for 4 channels, use PE0, PE1,PE2 & PE3 as the analog inputs,

results[0] always corresponds to the conversion from PE0.
for multi-channel conversions, depending on the number channels converted: 
results[1] -> PE1, results[2] -> PE2,results[3] -> PE3,
**********************************************************************************/


#ifndef ADMULTI
#define ADMULTI
// ADMulti.h
// Setup up ADC0 to convert up to 4 channels using SS2

#include <stdint.h>

// initialize the A/D converter to convert on 1-4 channels
void ADC_MultiInit(uint8_t HowMany);

//------------ADC_MultiRead------------
// Triggers A/D conversion and waits for result
// Input: none
// Output: up to 4 12-bit result of ADC conversions
// software trigger, busy-wait sampling, takes about 18.6uS to execute
// data returned by reference
// lowest numbered converted channel is in data[0]

void ADC_MultiRead(uint32_t data[4]);
#endif
