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
#include "Init.h"
#include "InputCapture.h"
#include "DrsSM.h"


/****************************************************************************
 Function
   Check4Keystroke
 Parameters
   None
 Returns
   bool: true if a new key was detected & posted
 Description
   checks to see if a new key from the keyboard is detected and, if so, 
   retrieves the key and posts an ES_NewKey event to TestHarnessService0
 Notes
   The functions that actually check the serial hardware for characters
   and retrieve them are assumed to be in ES_Port.c
   Since we always retrieve the keystroke when we detect it, thus clearing the
   hardware flag that indicates that a new key is ready this event checker 
   will only generate events on the arrival of new characters, even though we
   do not internally keep track of the last keystroke that we retrieved.
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/
bool Check4Keystroke(void)
{
  if ( IsNewKeyReady() ) // new key waiting?
  {
    ES_Event ThisEvent;
    ThisEvent.EventType = ES_NEW_KEY;
    ThisEvent.EventParam = GetNewKey();
    PostMapKeys( ThisEvent );
    return true;
  }
  return false;
}


static uint32_t EncoderGoal = 1000000;

bool Check4EncoderCount( void)
{
	uint32_t count = GetLEncoderCount();
	
	if (count > EncoderGoal)
	{
		ES_Event event;
		event.EventType = EV_ENCODER_GOAL;
		PostKartSM(event);
		
		return true;
	}
		return false;
}

void SetEncoderGoal (uint32_t goal)
{
	EncoderGoal = goal;
}

#define WAIT_FOR_START 0x00
#define FLAG_DROPPED	 0x01
#define CAUTION_FLAG	 0x02
#define RACE_OVER			 0x03

bool Check4RaceStatusChange( void )
{
	static uint8_t LastState = WAIT_FOR_START;
	uint8_t CurrentState = GetRaceStatus( MyKartNum );
	
	if (CurrentState != LastState)
	{
		if (CurrentState == FLAG_DROPPED)
		{
			ES_Event PostEvent;
			PostEvent.EventType = EV_GAME_START;
			PostKartSM(PostEvent);
			
			printf("\r\nFlag Dropped!");
		}
		else
		if (CurrentState == CAUTION_FLAG)
		{
			ES_Event PostEvent;
			PostEvent.EventType = EV_GAME_PAUSE;
			PostKartSM(PostEvent);
			printf("\r\fCaution!!");
		}
		else
		if (CurrentState == RACE_OVER)
		{
			ES_Event PostEvent;
			PostEvent.EventType = EV_GAME_OVER;
			PostKartSM(PostEvent);
			printf("\r\nRace Over");
		}
		LastState = CurrentState;
		return true;
	}
	
	return false;
}
