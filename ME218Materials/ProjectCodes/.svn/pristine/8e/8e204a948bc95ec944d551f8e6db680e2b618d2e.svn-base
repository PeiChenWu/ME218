/*****************************************************************************************************************
This module intends to communicate with the command generator for ME218b Project.
Frame Mode: SPO = 1, SPH = 1
Data Width: 8 bits
Bit Rate: ?? MHz (SysClk / (CPSDVSR * (1 + SCR)))
*****************************************************************************************************************/
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


static uint32_t receiveDataMsb;
static uint32_t receiveDataLsb;

void InitSSI(void)
{
	HWREG(SYSCTL_RCGCSSI) |= SYSCTL_RCGCSSI_R0;		// enable clock to the SSI Module 0
	
	// make sure that the PWM module clock has gotten going
	while ((HWREG(SYSCTL_PRSSI) & SYSCTL_PRSSI_R0) != SYSCTL_PRSSI_R0);
	
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
	HWREG(SSI0_BASE+SSI_O_CR0) |= (0xff << 8);		// Bit rate = 40/ (255 * (1 + 15))
	
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
	HWREG(GPIO_PORTA_BASE+GPIO_O_ODR) |= BIT3HI;	//Use Open Drain Output to be compatible with the device
	
	// Enable SSI module
	HWREG(SSI0_BASE+SSI_O_CR1) |= SSI_CR1_SSE;
}

void SendCommand(uint8_t command)
{
	// enable EOT interrupt
	HWREG(SSI0_BASE+SSI_O_IM) |= SSI_IM_EOTIM;
	
	HWREG(SSI0_BASE+SSI_O_DR) = command;
	HWREG(SSI0_BASE+SSI_O_DR) = 0x00;
	HWREG(SSI0_BASE+SSI_O_DR) = 0x00;
	HWREG(SSI0_BASE+SSI_O_DR) = 0x00;
	HWREG(SSI0_BASE+SSI_O_DR) = 0x00;
	HWREG(SSI0_BASE+SSI_O_DR) = 0x00;
	HWREG(SSI0_BASE+SSI_O_DR) = 0x00;
	HWREG(SSI0_BASE+SSI_O_DR) = 0x00;
	
	//printf("\r\nsend command: %u", command);
}

uint32_t GetCommandMsb(void)
{
	return receiveDataMsb;
}
uint32_t GetCommandLsb(void)
{
	return receiveDataLsb;
}

// EOT Interrupt Service Routine
void ISR_EndOfTransmit(void)
{
	// clear EOT interrupt
	HWREG(SSI0_BASE+SSI_O_ICR) = SSI_ICR_EOTIC;
	
	receiveDataMsb = 0;
	receiveDataLsb = 0;

	for (int i=0; i<4; i++)
	{
		uint8_t data = HWREG(SSI0_BASE+SSI_O_DR);
		receiveDataMsb |= data << ( (3-i) * 8 );
	}
	for (int i=4; i<8; i++)
	{
		uint8_t data = HWREG(SSI0_BASE+SSI_O_DR);
		receiveDataLsb |= data << ( (7-i) * 8 );
	}
}