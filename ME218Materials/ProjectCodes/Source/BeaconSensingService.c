/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ADMulti.h"
#include "Init.h"

#include "BeaconSensingService.h"

/*----------------------------- Module Defines ----------------------------*/
#define READ_SAMPLE_RATE 100			// Sample every 100ms
#define RESET_WAIT_RATE 1				// Wait 1ms to reset peak detection circuit 

/*---------------------------- Module Functions ---------------------------*/


/*---------------------------- Module Variables ---------------------------*/
static BeaconState_t CurrentState;
static uint8_t MyPriority;
static uint32_t BeaconValue;

/*------------------------------ Module Code ------------------------------*/

bool InitBeaconSensingService( uint8_t Priority)
{
	ES_Event ThisEvent;

	MyPriority = Priority;  // save our priority

	ES_Timer_InitTimer(BEACON_TIMER_SAMPLE, READ_SAMPLE_RATE);	

	return true;
}

bool PostBeaconSensingService( ES_Event ThisEvent )
{
	return ES_PostToService( MyPriority, ThisEvent);
}

ES_Event RunBeaconSensingService( ES_Event CurrentEvent )
{
   BeaconState_t NextState = CurrentState;
   ES_Event ReturnEvent = {ES_NO_EVENT, 0};

   switch ( CurrentState )
   {
       case ST_READ :
			 {
				 // Sample data every 100ms
				 if (CurrentEvent.EventType == ES_TIMEOUT && CurrentEvent.EventParam == BEACON_TIMER_SAMPLE)
				 {
						CurrentState = ST_RESET;
						uint32_t results[4];
	
						//Read the peak detection output
						ADC_MultiRead(results);
						BeaconValue = results[0];
						
					 //Reset peak detection circuit
						HWREG(BEACON_DATA) |= (BEACON_PIN); //Set PB5 HI
						ES_Timer_InitTimer(BEACON_TIMER_RESET, RESET_WAIT_RATE);
					 					 
						// Print out current beacon value for debugging
						printf("\r\n--------------------------Beacon Values----------------------------- \n");
						printf("\rBeacon Value: %i\n", BeaconValue);
					 
					} 
			 }
         break;	 
			 
			 case ST_RESET:
			 {
				 if (CurrentEvent.EventType == ES_TIMEOUT && CurrentEvent.EventParam == BEACON_TIMER_RESET)
				 {
					 //Enable peak detection circuit
					 HWREG(BEACON_DATA) &= ~(BEACON_PIN); //Set PB5 LO
					 ES_Timer_InitTimer(BEACON_TIMER_SAMPLE, READ_SAMPLE_RATE);	
				 }
				 
			 }
			 break;
	 }
			 
			 return(ReturnEvent); 
}

BeaconState_t QueryBeaconSensingService ( void )
{
   return(CurrentState);
}

uint32_t GetBeaconValue(void)
{
	return BeaconValue;
}