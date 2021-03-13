#include "StepperMotorService.h"

#include "PWMTiva.h"

#include <stdio.h>
#include "termio.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"

#define ALL_BITS (0xff<<2)

/*----------------------------- Module Defines ----------------------------*/
#define ONE_SEC 976
#define TEN_SEC (ONE_SEC *10)

//#define FULLSTEPDRIVE
//#define WAVEDRIVE
//#define HALFSTEPDRIVE
#define MICROSTEPDRIVE

// Model Constant

#ifdef FULLSTEPDRIVE
// Full Step Drive:
uint8_t StepTable[][4] = { {99, 0, 99, 0},
		{99, 0, 0, 99},
		{0, 99, 0, 99},
		{0, 99, 99, 0} };
#endif

#ifdef WAVEDRIVE
// Wave Drive:
uint8_t StepTable[][4] = { {99, 0, 0, 0},
		{0, 0, 0, 99},
		{0, 99, 0, 0},
		{0, 0, 99, 0} };
#endif
		
#ifdef HALFSTEPDRIVE
// Half Step Drive:
uint8_t StepTable[][4] = { {99, 0, 99, 0},
		{99, 0, 0, 0},
		{99, 0, 0, 99},
		{0, 0, 0, 99},
		{0, 99, 0, 99},
		{0, 99, 0, 0},
		{0, 99, 99, 0},
		{0, 0, 99, 0} };											
#endif
		
#ifdef MICROSTEPDRIVE
// Micro Step Drive:
uint8_t StepTable[][4] = { {71, 0, 71, 0},
		{92, 0, 38, 0},
		{99, 0, 0, 0},
		{92, 0, 0, 38},
		{71, 0, 0, 71},
		{38, 0, 0, 92},
		{0, 0, 0, 99},
		{0, 38, 0, 92},
		{0, 71, 0, 71},
		{0, 92, 0, 38},
		{0, 99, 0, 0},
		{0, 92, 38, 0},
		{0, 71, 71, 0},
		{0, 38, 92, 0},
		{0, 0, 99, 0},
		{38, 0, 92, 0}};
#endif

static uint16_t MotorSpeed;

static uint8_t MyPriority;

static uint8_t CurrentStep;
		
static bool Direction;

static void SendNextStep();
static uint16_t GetMotorSpeed(uint8_t SpeedLevel);

bool InitStepperMotorService( uint8_t Priority )
{
	ES_Event ThisEvent;
  
  MyPriority = Priority;
	
	// Initialize Direction Control Pin (PF0)
	HWREG(SYSCTL_RCGCGPIO) |= (SYSCTL_RCGCGPIO_R5);			// Enable PortF
	HWREG(GPIO_PORTF_BASE+GPIO_O_DEN) |= GPIO_PIN_4;		// Digital
	HWREG(GPIO_PORTF_BASE+GPIO_O_DIR) &= ~(GPIO_PIN_4);	// Input
	HWREG(GPIO_PORTF_BASE+GPIO_O_PUR) |= GPIO_PIN_4;		// Enable Pull-up Resistor
	
	// Initialize PWM
	PWM_TIVA_Init();
	
	PWM_TIVA_SetFreq(50, 0);
	PWM_TIVA_SetFreq(50, 1);
	
	PWM_TIVA_SetDuty(99, 0);	// PB6
	PWM_TIVA_SetDuty(99, 1);	// PB7
	PWM_TIVA_SetDuty(99, 2);	// PB4
	PWM_TIVA_SetDuty(99, 3);	// PB5
	
	MotorSpeed = 50;
	
	Direction = true;
	
	// Start Stepper Motor Timer
	ES_Timer_InitTimer(SM_TIMER, MotorSpeed);
	
	
  if (ES_PostToService( MyPriority, ThisEvent) == true)
  {
      return true;
  }else
  {
      return false;
  }
}


bool PostStepperMotorService( ES_Event ThisEvent )
{
		return ES_PostToService( MyPriority, ThisEvent);
}



ES_Event RunStepperMotorService( ES_Event ThisEvent )
{
	ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	if (ThisEvent.EventType == ES_TIMEOUT)
	{
		
		SendNextStep();
		
		// For evaluating maximum step rate
		/*
		static uint8_t StepCount = 100;
		StepCount --;
		if (StepCount == 0)
		{
			Direction = !Direction;
			printf("press anything to continue");
			getchar();
			StepCount = 100;
		}
		*/
		
		
		// Restart the timer
		ES_Timer_InitTimer(SM_TIMER, MotorSpeed);
	}
	else
	if (ThisEvent.EventType == UpdateSpeed)
	{
		uint8_t SpeedLevel = ThisEvent.EventParam;
		MotorSpeed = GetMotorSpeed(SpeedLevel);
		
		printf("\n\rMotor speed: %u", MotorSpeed);
	}
	else
	if (ThisEvent.EventType == UpdateDir)
	{
		Direction = !Direction;
	}
	return ReturnEvent;
}

void SendNextStep()
{
	uint8_t output[4] = {StepTable[CurrentStep][0], StepTable[CurrentStep][1], StepTable[CurrentStep][2], StepTable[CurrentStep][3]};
	
	PWM_TIVA_SetDuty(output[0], 2);	// PB4
	PWM_TIVA_SetDuty(output[1], 3);	// PB5
	
	PWM_TIVA_SetDuty(output[2], 0);	// PB6
	PWM_TIVA_SetDuty(output[3], 1);	// PB7
	
	if (Direction)
		CurrentStep ++;
	else
		CurrentStep --;
	
	if (Direction)
	{
	#ifdef FULLSTEPDRIVE
		if (CurrentStep == 4)
			CurrentStep = 0;
	#endif
		
	#ifdef WAVEDRIVE
		if (CurrentStep == 4)
			CurrentStep = 0;
	#endif
		
	#ifdef HALFSTEPDRIVE
		if (CurrentStep == 8)
			CurrentStep = 0;
	#endif
		
	#ifdef MICROSTEPDRIVE
		if (CurrentStep == 16)
			CurrentStep = 0;
	#endif
	}
	else
	{
	#ifdef FULLSTEPDRIVE
		if (CurrentStep == 255)
			CurrentStep = 3;
	#endif
		
	#ifdef WAVEDRIVE
		if (CurrentStep == 255)
			CurrentStep = 3;
	#endif
		
	#ifdef HALFSTEPDRIVE
		if (CurrentStep == 255)
			CurrentStep = 7;
	#endif
		
	#ifdef MICROSTEPDRIVE
		if (CurrentStep == 255)
			CurrentStep = 15;
	#endif
	}
}

uint16_t GetMotorSpeed(uint8_t Level)
{
	uint16_t returnValue;
	
	#ifdef FULLSTEPDRIVE
	returnValue = 10 * Level + 20;
	#endif
	
	#ifdef WAVEDRIVE
	returnValue = 10 * Level + 20;
	#endif
	
	#ifdef HALFSTEPDRIVE
	returnValue = 10 * Level + 10;
	#endif
	
	#ifdef MICROSTEPDRIVE
	returnValue = 4 * Level + 8;
	#endif
	
	return returnValue;
}