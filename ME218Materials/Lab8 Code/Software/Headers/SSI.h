#ifndef SSL_H
#define SSL_H

#include <stdint.h>

void InitSSI(void);
void queryCommand(void);		// Send 0xAA to command generator to query command

// End of transmit interrupt routine
void EOTRoutine(void);	// Recieve command from the command generator

uint8_t getCommand(void);


#endif