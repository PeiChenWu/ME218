/****************************************************************************
 Module
   TimeKeeping.c

 Revision
   2.0.1

 Description
   This is a template file for implementing state machines.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/07/13 21:00 jec      corrections to return variable (should have been
                         ReturnEvent, not CurrentEvent) and several EV_xxx
                         event names that were left over from the old version
 02/08/12 09:56 jec      revisions for the Events and Services Framework Gen2
 02/13/10 14:29 jec      revised Start and run to add new kind of entry function
                         to make implementing history entry cleaner
 02/13/10 12:29 jec      added NewEvent local variable to During function and
                         comments about using either it or Event as the return
 02/11/10 15:54 jec      more revised comments, removing last comment in during
                         function that belongs in the run function
 02/09/10 17:21 jec      updated comments about internal transitions on During funtion
 02/18/09 10:14 jec      removed redundant call to RunLowerlevelSM in EV_Entry
                         processing in During function
 02/20/07 21:37 jec      converted to use enumerated type for events & states
 02/13/05 19:38 jec      added support for self-transitions, reworked
                         to eliminate repeated transition code
 02/11/05 16:54 jec      converted to implment hierarchy explicitly
 02/25/03 10:32 jec      converted to take a passed event parameter
 02/18/99 10:19 jec      built template from MasterMachine.c
 02/14/99 10:34 jec      Began Coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "TimeKeeping.h"
#include "TimeDisplay.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE STATE_ZERO

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringDisplayTime( ES_Event Event);
static ES_Event DuringDisplayDate( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static TimeKeepingState_t CurrentState = STATE_Uninitialized;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
    RunTimeKeepingSM

 Parameters
   ES_Event: the event to process

 Returns
   ES_Event: an event to return

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 2/11/05, 10:45AM
****************************************************************************/
ES_Event RunTimeKeepingSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   TimeKeepingState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event

   switch ( CurrentState )
   {
       case STATE_DisplayTime :       // If current state is state one
         // Execute During function for DisplayTime. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringDisplayTime(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_ModeEVT : //If event is event one
                  // Execute action function for state one : event one
                  NextState = STATE_DisplayDate;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = true; //mark that we are taking a transition
                  break;
                // repeat cases as required for relevant events
            }
         }
         break;
      // repeat state pattern as required for other states
       case STATE_DisplayDate :       // If current state is state one
         // Execute During function for DisplayDate. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringDisplayDate(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_ModeEVT : //If event is event one
                  // Execute action function for state one : event one
                  NextState = STATE_DisplayTime;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = true; //mark that we are taking a transition
                  break;
            }
         }
         break;
    }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunTimeKeepingSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunTimeKeepingSM(EntryEventKind);
     }
     return(ReturnEvent);
}
/****************************************************************************
 Function
     StartTimeKeepingSM

 Parameters
     None

 Returns
     None

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 2/18/99, 10:38AM
****************************************************************************/
void StartTimeKeepingSM ( ES_Event CurrentEvent )
{
   // to implement entry to the history check to see if the CurrentState
   // variable has been changed from its initial value, if so then skip
   // forcing it to DisplayTime
   if ( STATE_Uninitialized == CurrentState )
   {
        CurrentState = STATE_DisplayTime;
   }
   // call the entry function (if any) for the ENTRY_STATE
   RunTimeKeepingSM(CurrentEvent);
}

/****************************************************************************
 Function
     QueryTimeKeepingSM

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     J. Edward Carryer, 2/11/05, 10:38AM
****************************************************************************/
TimeKeepingState_t QueryTimeKeepingSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringDisplayTime( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        DisplayTime();
        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        // ReturnEvent = RunLowerLevelSM(Event);
      
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}

static ES_Event DuringDisplayDate( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        DisplayDate();
        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        // ReturnEvent = RunLowerLevelSM(Event);
      
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}
