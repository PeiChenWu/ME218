#ifndef DRS_H
#define DRS_H

#include <stdint.h>

void InitSSI(void);
void SendCommand(uint8_t command);
uint32_t GetCommandMsb(void);
uint32_t GetCommandLsb(void);

// EOT Interrupt Service Routine
void ISR_EndOfTransmit(void);

#endif
