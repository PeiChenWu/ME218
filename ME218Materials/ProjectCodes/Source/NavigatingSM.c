/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "Init.h"
#include "DrsSM.h"

#include "KartSM.h"
#include "NavigatingSM.h"

#include "InputCapture.h"
#include "DcMotorService.h"
#include "EventCheckers.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE ST_Adjust_Orientation

#define MOVE_RPM 50
#define ROTATE_RPM 40
#define PULSE_PER_90_DEG 72
#define PULSE_PER_10_PIXEL 10

#define X_0 85
#define X_1 200
#define Y_0 40
#define Y_1 160
// center
#define X_c 145
#define Y_c 100

#define MAX_ORI_DIFF 10

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static int16_t GetDesOrientation( void );

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static NavigatingState_t CurrentState;
static uint16_t CurrentPos[2];
static int16_t CurrentOri;


/*------------------------------ Module Code ------------------------------*/

ES_Event RunNavigatingSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   NavigatingState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event

   switch ( CurrentState )
   {
		 case ST_Adjust_Orientation:
		 {
			 int16_t DesOri = GetDesOrientation();
			 printf("\r\nDesired Orientation: %d", DesOri);
			 printf("\r\nCurrent Orientation: %d", CurrentOri);
			 
			 if (CurrentEvent.EventType == ES_ENTRY)
			 {
				 printf("\r\nAdjust Orientation");
				 // Orientation error larger than threshold
				 bool BigError = false;
				 uint16_t Error = (CurrentOri - DesOri > 0) ? (CurrentOri - DesOri) : (DesOri - CurrentOri);
				 if (Error > MAX_ORI_DIFF && Error < 360 - MAX_ORI_DIFF)
				 {
					 BigError = true;					 
				 }
				 
				 // Make sure it always rotates across the possible smallest angle
				 if (BigError)
				 {
					 if (DesOri - CurrentOri > 0)
					 {
						 if (Error < 180)
						 {
							 // Left turn
							 SetLEncoderCount(0);
							 SetEncoderGoal(Error / 2);
							 AddU(Error);
							 Rotate(false, ROTATE_RPM);							 
						 }
						 else
						 {
							 // Right turn
							 SetLEncoderCount(0);
							 SetEncoderGoal(Error / 2);
							 AddU(-Error);
							 Rotate(true, ROTATE_RPM);							 
						 }
					 }
					 else
					 {
						 if (Error < 180)
						 {
							 // Right turn
							 SetLEncoderCount(0);
							 SetEncoderGoal(Error / 2);
							 AddU(-Error);
							 Rotate(true, ROTATE_RPM);							 
						 }
						 else
						 {
							 // Left turn
							 SetLEncoderCount(0);
							 SetEncoderGoal(Error / 2);
							 AddU(Error);
							 Rotate(false, ROTATE_RPM);							 
						 }						 
					 }
				 }
				 else
				 {
					 NextState = ST_Move;
					 MakeTransition = true;
				 }
			 }
			 if (CurrentEvent.EventType == EV_ENCODER_GOAL)
			 {
				 NextState = ST_Move;
				 MakeTransition = true;
				 Brake();
			 }
			 break;
		 }
		 case ST_Move:
		 {
			 if (CurrentEvent.EventType == ES_ENTRY)
			 {
				 printf("\r\nMove farward");
				 int16_t DesOri = GetDesOrientation();
				 uint16_t goal;
				 switch(DesOri)
				 {
					 case 180:
					 {
						 goal = (CurrentPos[0] - X_0) * PULSE_PER_10_PIXEL / 10;
						 break;
					 }
					 case 0:
					 {
						 goal = (X_1 - CurrentPos[0]) * PULSE_PER_10_PIXEL / 10;
						 break;
					 }
					 case 90:
					 {
						 goal = (CurrentPos[1] - Y_0) * PULSE_PER_10_PIXEL / 10;
						 break;
					 }
					 case 270:
					 {
						 goal = (Y_1 - CurrentPos[1]) * PULSE_PER_10_PIXEL / 10;
						 break;
					 }
					 default: 
						 goal = 60;
						 break;
				 }
				 // Move Forward
				 SetLEncoderCount(0);
				 SetEncoderGoal(goal);
				 Move (true, MOVE_RPM);
			 }
			 if (CurrentEvent.EventType == EV_ENCODER_GOAL)
			 {
				 Brake();
				 CurrentPos[0] = GetKartPositionX(MyKartNum);
				 CurrentPos[1] = GetKartPositionY(MyKartNum);
				 CurrentOri = GetKartOrientation(MyKartNum);
				 NextState = ST_Adjust_Orientation;
				 MakeTransition = true;
			 }
			 break;
		 }		 
   }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       // Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunNavigatingSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       // Execute entry function for new state
       // this defaults to ES_ENTRY
       RunNavigatingSM(EntryEventKind);
     }
		 
     return(ReturnEvent);
}

void StartNavigatingSM ( ES_Event CurrentEvent )
{
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = ENTRY_STATE;
   }
	 
   // call the entry function (if any) for the ENTRY_STATE
	 CurrentPos[0] = GetKartPositionX(MyKartNum);
	 CurrentPos[1] = GetKartPositionY(MyKartNum);
	 CurrentOri = GetKartOrientation(MyKartNum);
	 
   RunNavigatingSM(CurrentEvent);
}


NavigatingState_t QueryNavigatingSM ( void )
{
   return(CurrentState);
}



int16_t GetDesOrientation( void )
{
	int16_t result;
	// Zone 1
	if (CurrentPos[0] > X_0 && CurrentPos[1] < Y_0)
	{
		result = 180;
	}
	else
	// Zone 2
	if (CurrentPos[0] < X_0 && CurrentPos[1] < Y_1)
	{
		result = 270;
	}
	else
	// Zone 3
	if (CurrentPos[0] < X_1 && CurrentPos[1] > Y_1)
	{
		result = 0;
	}
	else
	// Zone 4
	if (CurrentPos[0] > X_1 && CurrentPos[1] > Y_0)
	{
		result = 90;
	}
	// else
	// Zone 5 (center zone)
	
	return result;
}

