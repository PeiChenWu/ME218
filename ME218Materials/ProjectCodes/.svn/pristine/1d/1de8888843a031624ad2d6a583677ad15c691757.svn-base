/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "Init.h"
#include "DrsSM.h"

#include "KartSM.h"
#include "ShootingSM.h"
#include "NavigatingSM.h"

#include "InputCapture.h"
#include "DcMotorService.h"

#include "BeaconSensingService.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE ST_ToShootingRegion
#define MAX_RPM 80

// definition for region
#define REGION_1_X 100
#define REGION_1_Y 100
#define REGION_2_X 100
#define REGION_2_Y 0
#define REGION_3_X 0
#define REGION_3_Y 0
#define REGION_4_X 0
#define REGION_4_Y 100

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringNavigating( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static ShootingState_t CurrentState;


/*------------------------------ Module Code ------------------------------*/

ES_Event RunShootingSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   ShootingState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = { ES_NO_EVENT, 0}; // consume event for checkpoint 3

   switch ( CurrentState )
   {
		 case ST_ToShootingRegion:
		 {
			 if (CurrentEvent.EventType == EV_SHOOTING)
			 {
				 // Go to the shooting position
				 NextState = ST_Navigating;
				 MakeTransition = true;
				 ReturnEvent.EventType = ES_NO_EVENT;		// Consume the event
			 }
			 break;
		 }
		 
		 case ST_Navigating:
		 {
			 CurrentEvent = DuringNavigating(CurrentEvent);

			 switch (CurrentEvent.EventType)
			 {
				// At the shooting position, align with the beacon
				case EV_ARRIVE_SZONE:
				{
					NextState = ST_AlignBeacon;
					MakeTransition = true;
				 
					EntryEventKind.EventType = ES_ENTRY;
					//ReturnEvent.EventType = ES_NO_EVENT;		// Consume the event
					break;
				}
				
				case ES_EXIT:
				{
					// When exiting navigating, stop the KART
					Brake();
					break;
				}
				
				case EV_GAME_OVER:
				{
					Brake();
					NextState = ST_Shooting_Idle;
					MakeTransition = true;
					break;
				}
				
				case EV_GAME_PAUSE:
				{
					Brake();
					NextState = ST_Shooting_Idle;
					MakeTransition = true;
					break;
				}
				default:
					ReturnEvent.EventType = ES_NO_EVENT;		// Consume the event
					break;
			}

			 break;
		 }
		 
		 case ST_AlignBeacon :
		 {
		// rotate until you hit the max A/D value (+/1 some error) that indicates alignment
			 break;
		 }
		 case ST_Loading:
		 {
			 			 
			 break;
		 }
		 
		 case ST_Shooting:
		 {
			 
			 break;
		 }
     
		 case ST_Shooting_Idle:
		 {
			 if (CurrentEvent.EventType == EV_GAME_START)
			 {
				 // Start the game when flag dropped
				 NextState = ST_Navigating;
				 MakeTransition = true;
				 ReturnEvent.EventType = ES_NO_EVENT;		// Consume the event
			 }
			 break;
		 }
	 }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       // Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunShootingSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       // Execute entry function for new state
       // this defaults to ES_ENTRY
       RunShootingSM(EntryEventKind);
     }
		 
     return(ReturnEvent);
}

void StartShootingSM ( ES_Event CurrentEvent )
{
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = ENTRY_STATE;
   }
	 
   // call the entry function (if any) for the ENTRY_STATE
   RunShootingSM(CurrentEvent);
}


ShootingState_t QueryShootingSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringNavigating( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ((Event.EventType == ES_ENTRY) || (Event.EventType == ES_ENTRY_HISTORY))
    {
        StartNavigatingSM( Event );
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        RunNavigatingSM(Event);
			
        // now do any local exit functionality
    }
		else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        ReturnEvent = RunNavigatingSM(Event);
     
        // do any activity that is repeated as long as we are in this state
    }

    return(ReturnEvent);
}
