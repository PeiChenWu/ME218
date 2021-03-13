/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "KartSM.h"
#include "RacingSM.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE ST_RACING

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringRacing( ES_Event Event);
static ES_Event DuringShooting( ES_Event Event);
static ES_Event DuringObsCrossing( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static uint8_t MyPriority;
static KartState_t CurrentState;


/*------------------------------ Module Code ------------------------------*/
bool InitKartSM ( uint8_t Priority )
{
	ES_Event ThisEvent;

	MyPriority = Priority;  // save our priority

	ThisEvent.EventType = ES_ENTRY;
	// Start the Master State machine

	StartKartSM( ThisEvent );

	return true;
}

bool PostKartSM( ES_Event ThisEvent )
{
	return ES_PostToService( MyPriority, ThisEvent);
}

ES_Event RunKartSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   KartState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = {ES_NO_EVENT, 0}; // assume we are not consuming event

   switch ( CurrentState )
   {
			 case ST_RACING:
			 {
				 // Run Racing state machine
				 CurrentEvent = DuringRacing(CurrentEvent);
				 
				 // Process Event
				 if (CurrentEvent.EventType != ES_NO_EVENT)
				 {
					 switch (CurrentEvent.EventType)
					 {
						 case EV_SHOOTING:
						 {
							 break;
						 }
						 case EV_OBS_CROSSING:
						 {
							 break;
						 }
						 default:
							 break;
					 }
				 }
				 
				 // Switch to shooting / obstacle crossing state when appropriate
				 break;
			 }
			 case ST_SHOOTING:
			 {
				 CurrentEvent = DuringShooting(CurrentEvent);
				 // Run Shooting state machine
				 break;
			 }
			 case ST_OBS_CROSSING:
			 {
				 CurrentEvent = DuringObsCrossing(CurrentEvent);
				 // Run Obstacle crossing state machine
				 break;
			 }
    }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunKartSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       // Execute entry function for new state
       // this defaults to ES_ENTRY
       RunKartSM(EntryEventKind);
     }
		
		 //printf("\r\n Running Kart SM");
		 
     return(ReturnEvent);
}

void StartKartSM ( ES_Event CurrentEvent )
{
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = ENTRY_STATE;
   }
	 
   // call the entry function (if any) for the ENTRY_STATE
   RunKartSM(CurrentEvent);
}


KartState_t QueryKartSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringRacing( ES_Event Event)
{
	ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption
	
	 if ((Event.EventType == ES_ENTRY) || (Event.EventType == ES_ENTRY_HISTORY))
    {
        // implement any entry actions required for this state machine
        
        // after that start any lower level machines that run in this state
        StartRacingSM( Event );
    }
    else if ( Event.EventType == ES_EXIT )
    {
        RunRacingSM(Event);

        // now do any local exit functionality
      
    }
		else
    {
        ReturnEvent = RunRacingSM(Event);
      
        // do any activity that is repeated as long as we are in this state
    }
	
	return ReturnEvent;
}

static ES_Event DuringShooting( ES_Event Event)
{
	ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption
	
	    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
   

	return ReturnEvent;
}

static ES_Event DuringObsCrossing( ES_Event Event)
{
	ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption
	return ReturnEvent;
}
