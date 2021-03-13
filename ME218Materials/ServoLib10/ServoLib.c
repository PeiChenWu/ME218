//#define TESTING
/****************************************************************************
 Module
     servolib.c

 Description
     servo output for testing
 Notes
     sets rollover period to 21.85mS ased on 24MHz clock.
 History
 When           Who     What/Why
 -------------- ---     --------
 11/04/10 18:37 jec     converted from ServoQD to a more proper library
 02/24/10 08:57 jec      move to TOC6 for Matt
 02/24/10 08:32 jec      corrected several errors: divider, TCTL2 &  TOV0
 11/10/09 14:19 jec      First stab
****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
#include <string.h>
#include <stdio.h>
#include <me218_c32.h>      /* common defines and macros for me218*/
#include "ServoLib.h"

/*--------------------------- External Variables --------------------------*/

/*----------------------------- Module Defines ----------------------------*/
#define INITIAL_POSITION 0xffff-4500
#define NumBitsPortT 8
/* BaseIndex is used to read mode string right to left */
#define BaseIndex (NumBitsPortT-1)
#define TicksPerMicroSec 3

/*------------------------------ Module Types -----------------------------*/
typedef struct  ChannelDef{
    unsigned char  IOSelVal;
    unsigned char * ModeReg;
    unsigned char  ModeVal;
    unsigned char ToggleBit;
    unsigned int * CompareReg;
}ChannelDef_t;
/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
ChannelDef_t Channels[NumBitsPortT] = {
            {_S12_IOS0, &TCTL2, (_S12_OM0 | _S12_OL0), _S12_TOV0, &TC0 },
            {_S12_IOS1, &TCTL2, (_S12_OM1 | _S12_OL1), _S12_TOV1, &TC1 },
            {_S12_IOS2, &TCTL2, (_S12_OM2 | _S12_OL2), _S12_TOV2, &TC2 },
            {_S12_IOS3, &TCTL2, (_S12_OM3 | _S12_OL3), _S12_TOV3, &TC3 },
            {_S12_IOS4, &TCTL1, (_S12_OM4 | _S12_OL4), _S12_TOV4, &TC4 },
            {_S12_IOS5, &TCTL1, (_S12_OM5 | _S12_OL5), _S12_TOV5, &TC5 },
            {_S12_IOS6, &TCTL1, (_S12_OM6 | _S12_OL6), _S12_TOV6, &TC6 },
            {_S12_IOS7, &TCTL1, (_S12_OM7 | _S12_OL7), _S12_TOV7, &TC7 }
};
char CurrentModeString[NumBitsPortT+1];

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
         Servo12_Init

 Parameters
         char [9] A null terminated string of 8 ASCII characters to describe
            the mode of each of the pins on Port T. the legal values are:
            'S' for servo output, 'x' for other function controlled by other
            modules.
            The string positions, reading left to right, corresspond to the
            pins MSB to LSB (modeString[0]=MSB, modeString[7]=LSB)

 Returns
         Servo12ReturnTyp
         SERVO12_Err if the input string is malformed
         SERVO12_OK if the mode string was OK

 Description
         Initializes the timer to a 3MHz count rate (assuming a 24MHz clock)
         and configures output compare system to take control of the selected
         pins and programs the toggle on overflow function to generate the
         pulse train with no software overhead.

 Notes
         Assumes a 24MHz bus clock, initializes to a 1.5mS pulse width

 Author
         J. Edward Carryer, 11/04/10 18:49
****************************************************************************/
Servo12ReturnTyp Servo12_Init( char * modeString )
{
    unsigned char i;
    static unsigned char InitPerformed = 0;

    if ( strlen(modeString) != NumBitsPortT)
        return SERVO12_Err;

    // make a local copy of the current mode string for future reference
    strcpy(CurrentModeString, modeString);

   /* init the timer subsystem if it has not already been done */
   if ( InitPerformed == 0) {
   // Program TCNT for a 21.85ms rollover rate
    TSCR1 |= _S12_TEN;            // enable the timer system
    TSCR2 = _S12_PR1 | _S12_PR0;  // 011 = divide by 8 from 24mHz = 3Mhz clk
    InitPerformed = 1;
   }

   /* scan through the string and program the selected pins as servo output */
    for ( i=0; i< NumBitsPortT; i++)
    {      /* step through the bits in the string, right to left */
        if ( modeString[BaseIndex-i]  == 'S')
        {
            // select channel for output compare
            TIOS |= Channels[i].IOSelVal;
            // program a rise on compare
            *(Channels[i].ModeReg) |= Channels[i].ModeVal;
            // set bit to toggleoverflow
            TTOV |= Channels[i].ToggleBit;
            // output pulse width for initial posiiton
            *(Channels[i].CompareReg) = INITIAL_POSITION;
        }
        else                            // not programmed as Servo
        {
            // remove pin control from timer
            *(Channels[i].ModeReg) &= ~Channels[i].ModeVal;
        }
    }
   return SERVO12_OK;
}

/****************************************************************************
 Function
         Servo12_SetPulseWidth

 Parameters
         unsigned char ChannelNum  0-7 to define which bit to change
         unsigned int NewWidth     width of the desired pulse in micro-seconds

 Returns
         Servo12ReturnTyp
         SERVO12_Err if the channel number is bad
         SERVO12_OK if the mode string was OK

 Description
         programs a new value for the pulse width accounting for the fact that
         the value that we program is 0xFFFF - DesiredWidth and the that
         timer is in timer ticks not microseconds.

 Notes
         Assumes a 24MHz bus clock

 Author
         J. Edward Carryer, 11/04/10 19:19
****************************************************************************/
Servo12ReturnTyp Servo12_SetPulseWidth( unsigned char ChannelNum, unsigned int NewWidth )
{
    // make sure the channel number is reasonable
    if ( ChannelNum >= NumBitsPortT)
        return SERVO12_Err;

    // make sure the channel number is configured for Servo control
    if ( CurrentModeString[BaseIndex - ChannelNum]  != 'S')
        return SERVO12_Err;
    // OK, good servo channel, program new value
    *(Channels[ChannelNum].CompareReg) = 0xFFFF - (NewWidth * TicksPerMicroSec);
}


#ifdef TESTING
void main(void)
{
    Servo12_Init("xxxxxxxS");
    // At this point, it should be outputting a 1.5mS pulse every 21.85ms
    // on all pins on Port T
    // you want the pulse to rise at the ned of the period, so that you get
    // a small pulse width. To do that program the rise time (the value that
    // you write into TC0 to be 0xffff - DesiredPulseWidth
    // 1 pulse-width tick = 0.3333us

		puts("Press any key to set Ch 0 to 1.5ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(0, 1500);
		puts("Channel 0 should now be at 1.5ms pulse width\n\r");

		/*puts("Press any key to set Ch 1 to 1ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(1, 1000);
		puts("Channel 1 should now be at 1ms pulse width\n\r");

		puts("Press any key to set Ch 2 to 1ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(2, 1000);
		puts("Channel 2 should now be at 1ms pulse width\n\r");

		puts("Press any key to set Ch 3 to 1ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(3, 1000);
		puts("Channel 3 should now be at 1ms pulse width\n\r");

		puts("Press any key to set Ch 4 to 1ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(4, 1000);
		puts("Channel 4 should now be at 1ms pulse width\n\r");
		
		puts("Press any key to set Ch 5 to 1ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(5, 1000);
		puts("Channel 5 should now be at 1ms pulse width\n\r");
		
		puts("Press any key to set Ch 6 to 1ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(6, 1000);
		puts("Channel 6 should now be at 1ms pulse width\n\r");
		
		puts("Press any key to set Ch 7 to 1ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(7, 1000);
		puts("Channel 7 should now be at 1ms pulse width\n\r");
		
		puts("Press any key to set Ch 0 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(0, 2000);
		puts("Channel 0 should now be at 2ms pulse width\n\r");
		
		puts("Press any key to set Ch 1 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(1, 2000);
		puts("Channel 1 should now be at 2ms pulse width\n\r");
		
		puts("Press any key to set Ch 2 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(2, 2000);
		puts("Channel 2 should now be at 2ms pulse width\n\r");
		
		puts("Press any key to set Ch 3 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(3, 2000);
		puts("Channel 3 should now be at 2ms pulse width\n\r");
		
		puts("Press any key to set Ch 4 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(4, 2000);
		puts("Channel 4 should now be at 2ms pulse width\n\r");
		
		puts("Press any key to set Ch 5 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(5, 2000);
		puts("Channel 5 should now be at 2ms pulse width\n\r");
		
		puts("Press any key to set Ch 6 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(6, 2000);
		puts("Channel 6 should now be at 2ms pulse width\n\r");
		
		puts("Press any key to set Ch 7 to 2ms pulse width\n\r");
		getchar();
		Servo12_SetPulseWidth(7, 2000);
		puts("Channel 7 should now be at 2ms pulse width\n\r");*/
		
 /*   while(1)
    {
    }*/
    
} 
#endif 