/****************************************************************************
 Module
   d:\me218bTNG\Lectures\Lecture31\SMTemplateMain.c

 Revision
   1.0.1

 Description
   This is a template file for a main() implementing state machines.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/08/12 10:32 jec      major re-work for the Events and Services Framework
                         Gen2
 03/03/10 00:36 jec      now that StartTemplateSM takes an event as input
                         you should pass it something.
 03/17/09 10:20 jec      added cast to return from RunTemplateSM() to quiet
                         warnings because now that function returns Event_t
 02/11/05 16:56 jec      Began coding
****************************************************************************/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_Timers.h"

void main (void)
{
  ES_Return_t ErrorType;
    
// Your hardware initialization function calls go here
   
// do not enable interrupts in ES_TimerInit to make it possible to single step 
// for demo/debug purposes
// this ruins the timing functionality, so put it back to run for real

// now initialize the Events and Services Framework and start it running
  ErrorType = ES_Initialize(ES_Timer_RATE_32MS);
  if ( ErrorType == Success ) {
    ErrorType = ES_Run();
  }
//if we got to here, there was an error
  switch (ErrorType){
    case FailedPointer:
      puts("Failed on NULL pointer");
      break;
    case FailedInit:
      puts("Failed Initialization");
      break;
    default:
      puts("Other Failure");
      break;
  }
  for(;;)   // hang after reporting error
    ;
}
