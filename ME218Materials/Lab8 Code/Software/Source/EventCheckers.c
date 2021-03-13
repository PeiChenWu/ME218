/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1 

 Description
   This is the sample for writing event checkers along with the event 
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.
   
 History
 When           Who     What/Why
 -------------- ---     --------
 08/06/13 13:36 jec     initial version
****************************************************************************/

// this will pull in the symbolic definitions for events, which we will want
// to post in response to detecting events
#include "ES_Configure.h"
// this will get us the structure definition for events, which we will need
// in order to post events in response to detecting events
#include "ES_Events.h"
// if you want to use distribution lists then you need those function 
// definitions too.
#include "ES_PostList.h"
// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "ES_ServiceHeaders.h"
// this test harness for the framework references the serial routines that
// are defined in ES_Port.c
#include "ES_Port.h"
// include our own prototypes to insure consistency between header & 
// actual functionsdefinition
#include "EventCheckers.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"

#include "SSI.h"

#define ALL_BITS (0xff<<2)


bool CheckNewCommand(void)
{
	static uint8_t LastCmd = 0xff;
	bool returnValue = false;
	
	//uint8_t CurrentCmd =getCommand();
	uint8_t CurrentCmd =0x09;
	
	
	if (CurrentCmd != LastCmd)
	{
		printf("\r\ncommand: %u", CurrentCmd);
		LastCmd = CurrentCmd;
		
		ES_Event PostEvent;
		PostEvent.EventType = NEW_CMD;
		PostEvent.EventParam = CurrentCmd;
		
		PostDCMotorSM(PostEvent);
		
		returnValue = true;
	}
	return returnValue;
}

bool CheckTapeSensor(void)
{
	static uint8_t LastState = 0;
	bool returnValue = false;
	
	uint8_t CurrentState = HWREG(GPIO_PORTC_BASE + (GPIO_O_DATA+ALL_BITS)) & BIT5HI;
	if (CurrentState != LastState)
	{
		if (CurrentState == BIT5HI)
		{
			ES_Event PostEvent;
			PostEvent.EventType = TAPE;
			
			PostDCMotorSM(PostEvent);
		}
	}	
}