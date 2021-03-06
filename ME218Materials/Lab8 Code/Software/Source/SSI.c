/***************************************************************
This module intends to communicate with the command generator
for ME218b Lab 8.
Frame Mode: SPO = 1, SPH = 1
Data Width: 8 bits
Bit Rate: < 1.9MHz (SysClk / (CPSDVSR * (1 + SCR)))
***************************************************************/
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ssi.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "bitdefs.h"

#include <stdio.h>
#include <termio.h>

#include "SSI.h"

#define ALL_BITS (0xff<<2)

uint8_t receiveData;


void InitSSI(void)
{
	HWREG(SYSCTL_RCGCSSI) |= SYSCTL_RCGCSSI_R0;		// enable clock to the SSI Module 0
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R0;	// enable clock to the GPIO Port A
	
	// make sure that the PWM module clock has gotten going
	while ((HWREG(SYSCTL_PRSSI) & SYSCTL_PRSSI_R0) != SYSCTL_PRSSI_R0)
    ;
	
	// configure the SSI clock to be the system clock
	HWREG(SSI0_BASE+SSI_O_CC) = 0; 
	
	// make sure SSI module is disabled when initializing
	HWREG(SSI0_BASE+SSI_O_CR1) &= ~SSI_CR1_SSE;
	
	// select master mode
	HWREG(SSI0_BASE+SSI_O_CR1) &= ~SSI_CR1_MS;
	
	// select End of Tranmit interrupt
	HWREG(SSI0_BASE+SSI_O_CR1) |= SSI_CR1_EOT;
	
	//configure clock pre-scaler to be 4
	HWREG(SSI0_BASE+SSI_O_CPSR) |= 0x04;
	
	// configure clock rate to make the bit rate
	HWREG(SSI0_BASE+SSI_O_CR0) |= (0x08 << 8);		// Bit rate = 40/ (4 * (1 + 8))
	
	// configure phase & polarity, data size and mode
	HWREG(SSI0_BASE+SSI_O_CR0) |= (SSI_CR0_SPH | SSI_CR0_SPO | SSI_CR0_DSS_8 | SSI_CR0_FRF_MOTO);

	// locally enable interrupt on TXRIS
	HWREG(SSI0_BASE+SSI_O_IM) |= SSI_IM_TXIM;
	
	// enable NVIC interrupt
	HWREG(NVIC_EN0) |= BIT7HI;
	
	// globally enable interrupt
	__enable_irq();
	
	// Configure GPIO (PA 2-5 are by default used for SSI0)
	HWREG(GPIO_PORTA_BASE+GPIO_O_AFSEL) |= (BIT2HI | BIT3HI | BIT4HI | BIT5HI);
	
	HWREG(GPIO_PORTA_BASE+GPIO_O_DEN) |= (BIT2HI | BIT3HI | BIT4HI | BIT5HI);
	HWREG(GPIO_PORTA_BASE+GPIO_O_DIR) |= (BIT2HI | BIT3HI | BIT5HI);
	HWREG(GPIO_PORTA_BASE+GPIO_O_DIR) &= ~BIT4HI;
	HWREG(GPIO_PORTA_BASE+GPIO_O_PUR) |= BIT2HI;	//Using Mode3 so program the pull-up on the clock line
	
	// Enable SSI module
	HWREG(SSI0_BASE+SSI_O_CR1) |= SSI_CR1_SSE;
	
	receiveData = 0x00ff;
}

// Send 0xAA to the command generator
void queryCommand(void)
{
	// enable EOT interrupt
	HWREG(SSI0_BASE+SSI_O_IM) |= SSI_IM_EOTIM;	
	
	HWREG(SSI0_BASE+SSI_O_DR) = 0xaa;
}

void EOTRoutine(void)
{
	// clear EOT interrupt
	HWREG(SSI0_BASE+SSI_O_ICR) = SSI_ICR_EOTIC;
	
	receiveData = HWREG(SSI0_BASE+SSI_O_DR);
	//printf("\r\nreceiveData: %u", receiveData);
}

uint8_t getCommand(void)
{
	return receiveData;
}