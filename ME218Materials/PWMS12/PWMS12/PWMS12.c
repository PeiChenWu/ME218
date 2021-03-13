#define TEST
/****************************************************************************
 Module
     pwms12.c

 Description
     This is a module implementing 3 channels of PWM on the 9S12C32
 Notes
     As implemented, resolution is fixed at 1%

 History
 When           Who     What/Why
 -------------- ---     --------
 11/13/04 16:34 jec      extended the test harness to cover the variable
                         periods too.
 06/15/04 17:35 jec      fixed bug in SetPeriod, prescale B was overwriting
                         the prescale A, should have been |=, not =
 06/15/04 08:49 jec      added code for SetPeriod & SetDuty
 06/07/04 18:09 jec      Began Coding
****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
#include <hidef.h>         /* common defines and macros */
#include <mc9s12c32.h>     /* derivative information */
#include <S12C32bits.h>    /* bit definitions  */
#include <PWMS12.h>
#include <stdio.h>
/*--------------------------- External Variables --------------------------*/

/*----------------------------- Module Defines ----------------------------*/
#define INITIAL_PERIOD 0x401E
#define MAX_CHANNELS   3
/* mask to isolate pre-sale value in PERIOD parameter */
#define PRESCALER_MASK  0xf000
/* shift to get that pre-scaler in the right place for pre-scale clock A */
#define PRESCALE_SHIFT 12
/* amount to shift the pre-scaler to get it moved for pre-scale clock B */
#define PRE_B_SHIFT    4
/* mask to isolate the scale value in the PERIOD parameter */
#define SCALER_MASK     0x03FF

/*------------------------------ Module Types -----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static unsigned char * const DutyRegisters[] = { &PWMDTY0, &PWMDTY1, &PWMDTY2};


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     PWMS12_Init

 Parameters
     none

 Returns
     None.

 Description
     Initializes the PWM subsystem to 500Hz, 1% resolution, 0%DC,
 Notes
     None.

 Author
     J. Edward Carryer, 06/07/04 14:00
****************************************************************************/
void PWMS12_Init(void)
{
   unsigned char i;
   PWMS12_SetPeriod(INITIAL_PERIOD, PWMS12_GRP0);
   PWMS12_SetPeriod(INITIAL_PERIOD, PWMS12_GRP1);
   /* set polarity to DC = high time */
   PWMPOL = (_S12_PPOL0 | _S12_PPOL1 | _S12_PPOL2);
   PWMPER0 = PWMS12_MAX_DC;
   PWMPER1 = PWMS12_MAX_DC;
   PWMPER2 = PWMS12_MAX_DC;
   for ( i=0; i< MAX_CHANNELS; i++)
   {
        PWMS12_SetDuty(0, i);  /* start all channels off at 0% */
   }
   /* map PWM to PORT T */
   MODRR |= (_S12_MODRR0 | _S12_MODRR1 | _S12_MODRR2 );
   /* enable the PWM subsytem on channels 0-2 */
   PWME |= (_S12_PWME0 | _S12_PWME1 | _S12_PWME2);

}

/****************************************************************************
 Function
     PWMS12_SetDuty

 Parameters
     unsigned char Num, the number of the timer to set.

 Returns
     PWMS12_ERR if requested timer does not exist
     PWMS12_OK  otherwise

 Description
     sets the duty cycle for a PWM channel.

 Notes
     None.

 Author
     J. Edward Carryer, 06/15/04 08:25
****************************************************************************/
signed char PWMS12_SetDuty(unsigned char dutyCycle, unsigned char channel)
{
    if ((channel >= MAX_CHANNELS) || (dutyCycle > PWMS12_MAX_DC))
        return PWMS12_ERR;
    else
        *DutyRegisters[channel] = dutyCycle;
    return PWMS12_OK;
}

/****************************************************************************
 Function
     PWMS12_SetPeriod

 Parameters
     unsigned short newPeriod, the new period to set. This is an encoded form
     of the constants to be programmed into the PWM pre-scaler & scaler. It
     should be one of the pre-defined constants or created as an unsigned short
     using the the form:
     pre-scale exponent(upper 4 bits) : scaler multiplier (lower 10 bits)
     the scaler multiplier should be a value between 0 & 512, with 0 indicating
     that the scaler should not be used and CLKx used directly.
     for example: with a 24MHz busclock for a 500Hz rate at 1% resolution the
     pre-scaler should be 16, exponent = 4. The scaler multiplier should be
     30. the constant should be: 0x401E
     with a 24MHz busclock for a 240kHz rate at 1% resolution, the pre-scaler
     should be 1 (exponent=0), no scale multiplier, use CLKx directly, the
     constant should be 0x0000

     unsigned char group, one of the two channel groups to which the new period
     should be applied. Channels 0 & 1 are in group 0 (PWMS12_GRP0), channels 2
     is in group 1 (PWMS12_GRP1)

 Returns
     signed char PWMS12_ERR for error PWMS12_OK for success

 Description
     decodes the period and programs the appropriate values into the pre-scaler,
     scaler and clock select bits.

 Notes
     None.

 Author
     J. Edward Carryer, 06/15/04 08:35
****************************************************************************/
signed char PWMS12_SetPeriod(unsigned short newPeriod, unsigned char group)
{
    unsigned char UseScaler = ((newPeriod & SCALER_MASK) != 0);
    unsigned char ScalerValue =
       (unsigned char)((newPeriod & SCALER_MASK) >> 1);
    unsigned char PreScaleValue =
       (unsigned char)((newPeriod & PRESCALER_MASK) >> PRESCALE_SHIFT);
//    printf("UseScaler %x, ScalerValue %x, PreScaleVlau %x \r\n", UseScaler, ScalerValue, PreScaleValue);
    if ( group == PWMS12_GRP0)
    {
        PWMCLK &= ~(_S12_PCLK0 | _S12_PCLK1);  /* choose clock A as default*/
        if ( UseScaler)
        {
            PWMCLK |= (_S12_PCLK0 | _S12_PCLK1);  /* choose clock SA */
            PWMSCLA = ScalerValue;
        }
        /* set pre-scaler for clock A */
        /* clear releavant bits first */
        PWMPRCLK &= ~(_S12_PCKA0 | _S12_PCKA1 | _S12_PCKA2);
        PWMPRCLK |= PreScaleValue;
    }
    else if ( group == PWMS12_GRP1)
    {
        PWMCLK &= ~(_S12_PCLK2 | _S12_PCLK3);  /* choose clock B as default*/
        if ( UseScaler)
        {
            PWMCLK |= (_S12_PCLK2 | _S12_PCLK3);  /* choose clock SB */
            PWMSCLB = ScalerValue;
        }
        /* set pre-scaler for clock b */
        PWMPRCLK &= ~(_S12_PCKB0 | _S12_PCKB1 |  _S12_PCKB2);
        PWMPRCLK |= PreScaleValue << PRE_B_SHIFT;
     }
     else
            return PWMS12_ERR;
     return PWMS12_OK;
}

/****************************************************************************
 Function
     PWMS12_End

 Parameters
     None.

 Returns
     None

 Description
     Disables the PWM sub-system.

 Notes
     Simply returns control of the lines to Port T. It does not force the
     outputs to a particular state, or even force the lines to be outputs.

 Author
     J. Edward Carryer, 06/015/04 09:45
****************************************************************************/
void PWMS12_End(void)
{
   /* undo map of PWM to PORT T */
   MODRR &= ~(_S12_MODRR0 | _S12_MODRR1 | _S12_MODRR2 );
   /* disable the PWM subsytem on channels 0-2 */
   PWME &= ~(_S12_PWME0 | _S12_PWME1 | _S12_PWME2);
}
/*------------------------------- Footnotes -------------------------------*/
#ifdef TEST

#include <stdio.h>
//#include < ME218_C32.h>
#include <timerS12.h>
#include "pwmS12.h"

#define STEP_DELAY 610
#define MIN_RAMP 0
#define MAX_RAMP 100

unsigned int Periods[]={
  PWMS12_4000US,
  PWMS12_3300US,
  PWMS12_2000US,
  PWMS12_1950US,
  PWMS12_1650US,
  PWMS12_1250US,
  PWMS12_1000US,
  PWMS12_825US,
  PWMS12_650US,
  PWMS12_500US
};

void main(void)
{
   unsigned char i;
   TMRS12_Init(TMRS12_RATE_8MS);
   PWMS12_Init();
   /* set channel 2 to 250Hz */
   PWMS12_SetPeriod(PWMS12_4000US, PWMS12_GRP1);
   printf("Ramping DC from %d-%d%% in 10%% steps at 5 second intervals\r\n",
         MIN_RAMP, MAX_RAMP);
   puts("Starting\r\n");
  /* ramp through duty cycles */
   for ( i=MIN_RAMP; i<=MAX_RAMP; i+=10)
   {
        PWMS12_SetDuty(i, PWMS12_CHAN0);
        PWMS12_SetDuty(i+10, PWMS12_CHAN1);
        PWMS12_SetDuty(i+20, PWMS12_CHAN2);
        printf("Outputting %d%%Duty Cycle\r",i);
        TMRS12_InitTimer(0, STEP_DELAY); /* STEP_DELAY  = 5s w/ 8.19mS interval */
        while(TMRS12_IsTimerExpired(0) != TMRS12_EXPIRED)
            ;
   }
   /* ramp through periods with 50% DC */

   PWMS12_SetDuty(50, PWMS12_CHAN0);
   PWMS12_SetDuty(50, PWMS12_CHAN1);
   PWMS12_SetDuty(50, PWMS12_CHAN2);
   for ( i=0; i<(sizeof(Periods)/sizeof(unsigned int)); i++)
   {
        printf("Outputting period #%d\r",i);
        PWMS12_SetPeriod(Periods[i], PWMS12_GRP1);
        PWMS12_SetPeriod(Periods[i], PWMS12_GRP0);
        TMRS12_InitTimer(0, STEP_DELAY); /* STEP_DELAY  = 5s w/ 8.19mS interval */
        while(TMRS12_IsTimerExpired(0) != TMRS12_EXPIRED)
            ;
   }
   PWMS12_End();
   TMRS12_InitTimer(0, STEP_DELAY); /* STEP_DELAY  = 5s w/ 8.19mS interval */
   while(TMRS12_IsTimerExpired(0) != TMRS12_EXPIRED)
      ;
}
#endif
/*------------------------------ End of file ------------------------------*/

