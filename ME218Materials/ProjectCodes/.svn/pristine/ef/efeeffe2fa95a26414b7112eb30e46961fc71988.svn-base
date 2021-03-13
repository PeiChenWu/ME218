/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "Init.h"
#include "DrsSM.h"

#include "KartSM.h"
#include "RacingSM.h"
#include "NavigatingSM.h"

#include "InputCapture.h"
#include "DcMotorService.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE ST_Racing_Idle
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
static RacingState_t CurrentState;


/*------------------------------ Module Code ------------------------------*/

ES_Event RunRacingSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   RacingState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = { ES_NO_EVENT, 0}; // consume event for checkpoint 3

   switch ( CurrentState )
   {
		 case ST_Racing_Idle:
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
		 case ST_Navigating :
		 {
			 CurrentEvent = DuringNavigating(CurrentEvent);

			 switch (CurrentEvent.EventType)
			 {
				// Check race status and shooting status when arrived at region 1
				case EV_ARRIVE_R1:
				{
					NextState = ST_CheckGameStatus;
					MakeTransition = true;
				 
					EntryEventKind.EventType = ES_ENTRY;
					ReturnEvent.EventType = ES_NO_EVENT;		// Consume the event
					break;
				}
				case EV_ARRIVE_R2:
				{
					NextState = ST_CheckObstacleStatus;
					MakeTransition = true;
					
					EntryEventKind.EventType = ES_ENTRY;
					ReturnEvent.EventType = ES_NO_EVENT;		// Consume the event
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
					NextState = ST_Racing_Idle;
					MakeTransition = true;
					break;
				}
				case EV_GAME_PAUSE:
				{
					Brake();
					NextState = ST_Racing_Idle;
					MakeTransition = true;
					break;
				}
				default:
					ReturnEvent.EventType = ES_NO_EVENT;		// Consume the event
					break;
			}

			 
			 break;
		 }
		 case ST_CheckGameStatus:
		 {
			 // When close to region 1, check game status and decide to go to shooting zone or not
			 if (CurrentEvent.EventType == ES_ENTRY)
			 {
				 // Check how many laps left
				 if (GetRemainingLaps(MyKartNum) == 0)
				 {
					 // Already finished all laps
					 NextState = ST_Racing_Idle;
					 MakeTransition = true;
				 }
				 // Decide whether to transit to shooting or not
				 if (!IsTargetHit(MyKartNum))
				 {
					 ReturnEvent.EventType = EV_SHOOTING;		// Remap event
				 }
			 }
			 
			 break;
		 }
		 case ST_CheckObstacleStatus:
		 {
			 // When close to region 2, check obstacle status and decide to cross obstacle or not
			 			 // When close to region 1, check game status and decide to go to shooting zone or not
			 if (CurrentEvent.EventType == ES_ENTRY)
			 {
				 // Decide whether to transit to shooting or not
				 if (!IsObstacleDone(MyKartNum))
				 {
					 ReturnEvent.EventType = EV_OBS_CROSSING;		// Remap event
				 }
			 }
			 
			 break;
		 }
    }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       // Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunRacingSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       // Execute entry function for new state
       // this defaults to ES_ENTRY
       RunRacingSM(EntryEventKind);
     }
		 
     return(ReturnEvent);
}

void StartRacingSM ( ES_Event CurrentEvent )
{
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = ENTRY_STATE;
   }
	 
   // call the entry function (if any) for the ENTRY_STATE
   RunRacingSM(CurrentEvent);
}


RacingState_t QueryRacingSM ( void )
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
