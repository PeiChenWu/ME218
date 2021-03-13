#include "DCMotorSM.h"

#include <stdio.h>
#include "termio.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"

#include "PWM.h"
#include "SSI.h"
#include "InputCapture.h"

#define ALL_BITS (0xff<<2)

/*----------------------------- Module Defines ----------------------------*/
#define ONE_SEC 976
#define TEN_SEC (ONE_SEC *10)
#define QUERY_RATE 500

#define LEFT_MOTOR_DIR BIT2HI
#define RIGHT_MOTOR_DIR BIT3HI

#define FULL_SPEED_LEFT 100
#define HALF_SPEED_LEFT 50
#define FULL_SPEED_RIGHT 100
#define HALF_SPEED_RIGHT 50

#define TIME_ROTATE_90 1200
#define TIME_ROTATE_45 600


/***************************************************************************
Command Byte Definition
***************************************************************************/
#define STOP 					0x00
#define ROTATE_90C 		0x02
#define ROTATE_45C		0x03
#define ROTATE_90CC 	0x04
#define ROTATE_45CC 	0x05
#define FORWARD_HALF 	0x08
#define FORWARD_FULL 	0x09
#define REVERSE_HALF 	0x10
#define REVERSE_FULL 	0x11
#define BEACON_ALIGN 	0x20
#define STOP_AT_TAPE	0x40
#define NEW_COMMAND		0xff
/***************************************************************************

***************************************************************************/


static uint8_t MyPriority;
static uint8_t CurrentState;
static uint8_t CurrentCmd;

static uint32_t PeriodBeacon;
		
static void processCommand(void);
static void executeCommand(void);

/******************************************************************************
Command Motors
******************************************************************************/
// true: rotate clockwaise; false: rotate couter-clockwise
static void Rotate(bool Dir);	

// Dir - true: move forward; false: move in reverse
// Speed - true: full speed; false: half speed
static void Move(bool Dir, bool Speed);			

static void Stop(void);
/***************************************************************************

***************************************************************************/


bool InitDCMotorSM( uint8_t Priority )
{
	ES_Event ThisEvent;
  
  MyPriority = Priority;
	
	
	// Initialize PWM for controlling motor speed
	InitPWM();
	
	// Initialize Pins (PD2, PD3) for controlling motor direction
	// PortD clock is already enabled in InitPWM()
	
	HWREG(GPIO_PORTD_BASE+GPIO_O_DEN) |= (LEFT_MOTOR_DIR | RIGHT_MOTOR_DIR);
	HWREG(GPIO_PORTD_BASE+GPIO_O_DIR) |= (LEFT_MOTOR_DIR | RIGHT_MOTOR_DIR);
	HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~(LEFT_MOTOR_DIR | RIGHT_MOTOR_DIR);
	
	
	InitSSI();
	CurrentCmd = 0xff;
	
	InitInputCapture();
	PeriodBeacon = 32000;
	
	// Initialize pin for tape event checker
	HWREG(GPIO_PORTC_BASE+GPIO_O_DEN) |= BIT5HI;
	HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) &= ~BIT5HI;
	
	ES_Timer_InitTimer(DC_TIMER, QUERY_RATE);
	
	CurrentState = initState;
		
  if (ES_PostToService( MyPriority, ThisEvent) == true)
  {
      return true;
  }else
  {
      return false;
  }
}


bool PostDCMotorSM( ES_Event ThisEvent )
{
		return ES_PostToService( MyPriority, ThisEvent);
}



ES_Event RunDCMotorSM( ES_Event ThisEvent )
{
	ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	if (ThisEvent.EventType == NEW_CMD)
	{
		CurrentCmd = ThisEvent.EventParam;
		processCommand();
		printf("\r\nstate: %u", CurrentState);
	}
	else
	if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DC_TIMER)
	{
		queryCommand();
		ES_Timer_InitTimer(DC_TIMER, QUERY_RATE);
	}
	
	
	switch (CurrentState)
	{
		case initState:
		{
			break;
		}
		case alignBeacon:
		{
			if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == ROTATE_TIMER)
			{
				uint32_t currentPeriod = GetPeriod();
				printf("\r\ncurrent period: %u", currentPeriod);
			
				// logic tested with manual 1.25kHz signal from TIVA pwm
				static uint8_t count = 0;
				if (currentPeriod - PeriodBeacon < 3000 || PeriodBeacon - currentPeriod < 3000)
				{
					count ++;
				
					// Consider beacon detected if get more than 10 pulse input
					if (count == 5)
					{
						printf("\r\nBeacon detected!");
						Stop();
						
						// disable input capture for beacon ?
						
						CurrentState = initState;
					}
				}
				else
				{
					count = 0;
				}
				
				ES_Timer_InitTimer(ROTATE_TIMER, 10);
			}
			break;
		}
		case detectTape:
		{
			if (ThisEvent.EventType == TAPE)
			{
				printf("\r\ntape detected!");
				
				Stop();
				CurrentState = initState;
			}
			break;
		}
		case executeCmd:
		{
			if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == ROTATE_TIMER)
			{
				Stop();
			}
			break;
		}
	}	
	
	return ReturnEvent;
}

void processCommand(void)
{
	if ( CurrentCmd == BEACON_ALIGN)
	{
		CurrentState = alignBeacon;
		
		// use the timer for sampling signal period
		ES_Timer_InitTimer(ROTATE_TIMER, 10);
		
		// Enable Input Capture of beacon alignment
		// start rotating the robot
		EnableInputCapture(true, true);
		Rotate(true);
	}
	else
	if ( CurrentCmd == STOP_AT_TAPE)
	{
		CurrentState = detectTape;
		
		// Enable Input Capture of tape detection
		//EnableInputCapture(true, false);
		Move(true, true);		
	}
	else
	if ( CurrentCmd == NEW_COMMAND)
	{
		CurrentState = initState;
	}
	else
	{
		CurrentState = executeCmd;
		executeCommand();		
	}	
}

void executeCommand(void)
{
	switch(CurrentCmd)
	{
		case STOP:
		{
			Stop();
			break;
		}
		case ROTATE_90C:
		{
			ES_Timer_InitTimer(ROTATE_TIMER, TIME_ROTATE_90);
			Rotate(true);
			break;
		}
		case ROTATE_45C:
		{
			ES_Timer_InitTimer(ROTATE_TIMER, TIME_ROTATE_45);
			Rotate(true);
			break;
		}
		case ROTATE_90CC:
		{
			ES_Timer_InitTimer(ROTATE_TIMER, TIME_ROTATE_90);
			Rotate(false);
			break;
		}
		case ROTATE_45CC:
		{
			ES_Timer_InitTimer(ROTATE_TIMER, TIME_ROTATE_45);
			Rotate(false);
			break;
		}
		case FORWARD_HALF:
		{
			Move(true, false);
			break;
		}
		case FORWARD_FULL:
		{
			Move(true, true);
			break;
		}
		case REVERSE_HALF:
		{
			Move(false, false);
			break;
		}
		case REVERSE_FULL:
		{
			Move(false, true);
			break;
		}
		default:
			Stop();
	}
}

void Rotate(bool Dir)
{
	if (Dir)
	{
		
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~RIGHT_MOTOR_DIR;
		setPWMPolarity(!Dir, RightMotor);
		
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) |= LEFT_MOTOR_DIR;
		setPWMPolarity(Dir, LeftMotor);
	}
	else
	{
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) |= RIGHT_MOTOR_DIR;
		setPWMPolarity(!Dir, RightMotor);
		
		
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~LEFT_MOTOR_DIR;
		setPWMPolarity(Dir, LeftMotor);
	}
	
	setPWMDuty(FULL_SPEED_LEFT, LeftMotor);
	setPWMDuty(FULL_SPEED_RIGHT, RightMotor);
}

void Move(bool Dir, bool Speed)
{
	if (Dir)
	{
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~RIGHT_MOTOR_DIR;
		//HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) |= RIGHT_MOTOR_DIR;
		setPWMPolarity(!Dir, RightMotor);
		
		//HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~LEFT_MOTOR_DIR;
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) |= LEFT_MOTOR_DIR;
		setPWMPolarity(!Dir, LeftMotor);
	}
	else
	{
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) |= RIGHT_MOTOR_DIR;
		setPWMPolarity(!Dir, RightMotor);
		
		HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) |= LEFT_MOTOR_DIR;
		setPWMPolarity(!Dir, LeftMotor);
	}
	
	if (Speed)
	{
		setPWMDuty(FULL_SPEED_LEFT, LeftMotor);
		setPWMDuty(FULL_SPEED_RIGHT, RightMotor);
	}
	else
	{
		setPWMDuty(HALF_SPEED_LEFT, LeftMotor);
		setPWMDuty(HALF_SPEED_RIGHT, RightMotor);
	}
}

void Stop()
{
	HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~(LEFT_MOTOR_DIR | RIGHT_MOTOR_DIR);
	setPWMPolarity(false, LeftMotor);
	setPWMPolarity(false, RightMotor);
	setPWMDuty(0, LeftMotor);
	setPWMDuty(0, RightMotor);
}