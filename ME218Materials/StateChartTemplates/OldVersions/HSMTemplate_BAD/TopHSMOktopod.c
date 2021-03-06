/****************************************************************************
 Module
   TopHSMOktopod.c

 Revision
   2.0.1

 Description
   This is a template for the top level Hierarchical state machine

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/08/12 01:39 jec      converted from MW_MasterMachine.c
 02/06/12 22:02 jec      converted to Gen 2 Events and Services Framework
 02/13/10 11:54 jec      converted During functions to return Event_t
                         so that they match the template
 02/21/07 17:04 jec      converted to pass Event_t to Start...()
 02/20/07 21:37 jec      converted to use enumerated type for events
 02/21/05 15:03 jec      Began Coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "TopHSMOktopod.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
static ES_Event DuringOktopod( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, though if the top level state machine
// is just a single state container for orthogonal regions, you could get
// away without it
static OktopodState_t CurrentState;
// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitOktopodSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     boolean, False if error in initialization, True otherwise

 Description
     Saves away the priority,  and starts
     the top level state machine
 Notes

 Author
     J. Edward Carryer, 02/06/12, 22:06
****************************************************************************/
boolean InitOktopodSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;  // save our priority

  ThisEvent.EventType = ES_ENTRY;
  // Start the Oktopod State machine

  StartOktopodSM( ThisEvent );

  return True;
}

/****************************************************************************
 Function
     PostOktopodSM

 Parameters
     ES_Event ThisEvent , the event to post to the queue

 Returns
     boolean False if the post operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
boolean PostOktopodSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunOktopodSM

 Parameters
   ES_Event: the event to process

 Returns
   ES_Event: an event to return

 Description
   the run function for the top level state machine 
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 02/06/12, 22:09
****************************************************************************/
// make recursive call warning into info
#pragma MESSAGE INFORMATION C1855
ES_Event RunOktopodSM( ES_Event CurrentEvent )
{

   // start by passing events to the lower level machines
   // this is the 'during' function for this machine
   CurrentEvent = DuringOktopod(CurrentEvent);
   // in the absence of an error the top level state machine should
   // always return ES_NO_EVENT
   CurrentEvent.EventType = ES_NO_EVENT;
   return(CurrentEvent);
}
/****************************************************************************
 Function
     StartOktopodSM

 Parameters
     ES_Event CurrentEvent

 Returns
     nothing

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 02/06/12, 22:15
****************************************************************************/
void StartOktopodSM ( ES_Event CurrentEvent )
{
  // local variable to get debugger to display the value of CurrentEvent
  volatile ES_Event LocalEvent = CurrentEvent;
  // if there is more than 1 state to the top level machine you will need 
  // to initialize the state variable
  CurrentState = Calibrating;
  // now we need to let the Run function init the lower level state machines
  // use LocalEvent to keep the compiler from complaining about unused var
  RunOktopodSM(LocalEvent);
  return;
}


/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringOktopod( ES_Event Event )
{
    // process ES_ENTRY & ES_EXIT events
    if ( Event.EventType == ES_ENTRY)
    {
        // implement any entry actions required for this state machine
        // after that start any lower level machines that run in this state
        StartCalibratingSM(Event);
    }else if (Event.EventType == ES_EXIT)
    {
      //give lower level state machines a chance clean up
      RunActionSM(Event);
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        RunTimerSM(Event);
        // repeat for any concurrent lower level machines
        RunMagControlSM(Event);
    }
    return(Event);
}
