#define KALMAN_FILTER


/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "DrsSM.h"
#include "SSI.h"
#include "Init.h"

/*----------------------------- Module Defines ----------------------------*/
#define DRS_SAMPLE_RATE 50			// Sample every 100ms
#define DRS_QUERY_RATE 3				// Wait 2ms before byte transfer

// macro definition for DRS command
#define CMD_GAME_STATUS 0x3f
#define CMD_KART_1 			0xc3
#define CMD_KART_2			0x5a
#define CMD_KART_3			0x7e

/*---------------------------- Module Functions ---------------------------*/


/*---------------------------- Module Variables ---------------------------*/
static DrsState_t CurrentState;
static uint8_t MyPriority;

static uint8_t KartStatus[3];

static uint16_t KartPositionX[3];
static uint8_t	KartPosMsbX[3];
static uint16_t KartPositionY[3];
static uint8_t	KartPosMsbY[3];
static int16_t KartOrientation[3];
static int8_t 	KartOriMsb[3];

#ifdef KALMAN_FILTER
static float X;		// value
static float P;		// estimation covariance error
static float K;		// Kalman gain
static float R;		// measurement noise
static float Q;		// process noise
static float U;		// control signal

#endif

/*------------------------------ Module Code ------------------------------*/

bool InitDrsSM( uint8_t Priority)
{
	ES_Event ThisEvent;

	MyPriority = Priority;  // save our priority

	ES_Timer_InitTimer(DRS_TIMER_SAMPLE, DRS_SAMPLE_RATE);
	
	#ifdef KALMAN_FILTER
	X = 180;		// Initial value
	P = 1;			// Initial error
	U = 0;
	R = 20;
	Q = 5; 
	#endif

	return true;
}

bool PostDrsSM( ES_Event ThisEvent )
{
	return ES_PostToService( MyPriority, ThisEvent);
}

ES_Event RunDrsSM( ES_Event CurrentEvent )
{
   DrsState_t NextState = CurrentState;
   ES_Event ReturnEvent = {ES_NO_EVENT, 0};

   switch ( CurrentState )
   {
       case ST_DRS_IDLE :
			 {
				 // Sample data every 100ms
				 if (CurrentEvent.EventType == ES_TIMEOUT && CurrentEvent.EventParam == DRS_TIMER_SAMPLE)
				 {
					 CurrentState = ST_GAME_STATUS;
					 ES_Timer_InitTimer(DRS_TIMER_SAMPLE, DRS_SAMPLE_RATE);
					 
					 #ifdef KALMAN_FILTER
					 // Prediction update
					 X = X + U;
					 P = P + Q;
					 
					 // Measurement update
					 K = P / (P + R);
					 X = X + K * (KartOrientation[MyKartNum] - X);
					 P = (1 - K) * P;
					 
					 // Update value (only for my kart)
					 KartOrientation[MyKartNum] = X;
					 #endif
					 
					 // Print out current status for debugging
					 for (int i=0; i<1; i++)
					 {
						 printf("\r\n--------------------------Kart %u----------------------------- \n", i+1);
						 printf("\rRace Status: %d\n", KartStatus[i]);
						 printf("\rPosition: (%d, %d)\n", KartPositionX[i], KartPositionY[i]);
						 printf("\rOrientation: %u\n", KartOrientation[i]);
					 }
					 
					 // Start from querying game status
					 SendCommand(CMD_GAME_STATUS);
					 ES_Timer_InitTimer(DRS_TIMER_QUERY, DRS_QUERY_RATE);
				 }
         break;
			 }
			 case ST_GAME_STATUS:
			 {
				 // Process Game Status Data
				 if (CurrentEvent.EventType == ES_TIMEOUT && CurrentEvent.EventParam == DRS_TIMER_QUERY)
				 {
					 uint32_t DataMsb = GetCommandMsb();
					 uint32_t DataLsb = GetCommandLsb();
					 
					 uint8_t CurrentByte;
					 for (int i = 0; i<4; i++)
					 {
						 switch (i)
						 {
							 case 3: 
								 CurrentByte = ( DataMsb >> ((3-i) * 8) );
								 KartStatus[0] = CurrentByte;
								 break;
							 default:
								 break;
						 }
					 }
					 for (int i = 4; i<8; i++)
					 {
						 switch (i)
						 {
							 case 4:
								 CurrentByte = ( DataLsb >> ((7-i) * 8) );
								 KartStatus[1] = CurrentByte;
								 break;
							 case 5:
								 CurrentByte = ( DataLsb >> ((7-i) * 8) );
								 KartStatus[2] = CurrentByte;
								 break;
							 default:
								 break;
						 }
					 }
				 
					 // Query Kart 1 information
					 CurrentState = ST_KART_1;
					 SendCommand(CMD_KART_1);
					 
					 ES_Timer_InitTimer(DRS_TIMER_QUERY, DRS_QUERY_RATE);
				 }
				 break;
			 }
			 case ST_KART_1:
			 {
				 // Process Kart 1 information
				 if (CurrentEvent.EventType == ES_TIMEOUT && CurrentEvent.EventParam == DRS_TIMER_QUERY)
				 {
					 uint32_t DataMsb = GetCommandMsb();
					 uint32_t DataLsb = GetCommandLsb();
					 for (int i = 0; i<4; i++)
					 {
						 uint8_t CurrentByte = ( DataMsb >> ((3-i) * 8) );
						 switch (i)
						 {
							case 2:
								KartPosMsbX[0] = CurrentByte;
								break;
							case 3:
								KartPositionX[0] = (KartPosMsbX[0] << 8) | CurrentByte;
								break;
							}
						}
						for (int i = 4; i<8; i++)
						{
							 uint8_t CurrentByte = ( DataLsb >> ((7-i) * 8) );
							 switch (i)
							 {						
								case 4:
									KartPosMsbY[0] = CurrentByte;
									break;
								case 5:
									KartPositionY[0] = (KartPosMsbY[0] << 8) | CurrentByte;
									break;
								case 6:
									KartOriMsb[0] = CurrentByte;
								case 7:
									KartOrientation[0] = (KartOriMsb[0] << 8) | CurrentByte;
								default:
									break;
							 }
						 }
						
						 if (KartOrientation[0] < 0) KartOrientation[0] += 360;

						// Query Kart 2 information
						CurrentState = ST_KART_2;
						SendCommand(CMD_KART_2);
					 
						ES_Timer_InitTimer(DRS_TIMER_QUERY, DRS_QUERY_RATE);
					}
				 break;
				}
			 case ST_KART_2:
			 {
				 // Process Kart 2 information
				 if (CurrentEvent.EventType == ES_TIMEOUT && CurrentEvent.EventParam == DRS_TIMER_QUERY)
				 {
					 uint32_t DataMsb = GetCommandMsb();
					 uint32_t DataLsb = GetCommandLsb();
					 for (int i = 0; i<4; i++)
					 {
						 uint8_t CurrentByte = ( DataMsb >> ((3-i) * 8) );
						 switch (i)
						 {
							case 2:
								KartPosMsbX[1] = CurrentByte;
								break;
							case 3:
								KartPositionX[1] = (KartPosMsbX[1] << 8) | CurrentByte;
								break;
							}
						}
						for (int i = 4; i<8; i++)
						{
							 uint8_t CurrentByte = ( DataLsb >> ((7-i) * 8) );
							 switch (i)
							 {						
								case 4:
									KartPosMsbY[1] = CurrentByte;
									break;
								case 5:
									KartPositionY[1] = (KartPosMsbY[1] << 8) | CurrentByte;
									break;
								case 6:
									KartOriMsb[1] = CurrentByte;
								case 7:
									KartOrientation[1] = (KartOriMsb[1] << 8) | CurrentByte;
								default:
									break;
							 }
						 }
						if (KartOrientation[1] < 0) KartOrientation[1] += 360;

						// Query Kart 3 information
						CurrentState = ST_KART_3;
						SendCommand(CMD_KART_2);
					 
						ES_Timer_InitTimer(DRS_TIMER_QUERY, DRS_QUERY_RATE);
					}
				 break;
			 }
			 case ST_KART_3:
			 {
				 // Process Kart 3 information
				 if (CurrentEvent.EventType == ES_TIMEOUT && CurrentEvent.EventParam == DRS_TIMER_QUERY)
				 {
					 uint32_t DataMsb = GetCommandMsb();
					 uint32_t DataLsb = GetCommandLsb();
					 for (int i = 0; i<4; i++)
					 {
						 uint8_t CurrentByte = ( DataMsb >> ((3-i) * 8) );
						 switch (i)
						 {
							case 2:
								
								KartPosMsbX[2] = CurrentByte;
								break;
							case 3:
								KartPositionX[2] = (KartPosMsbX[2] << 8) | CurrentByte;
								break;
							}
						}
						for (int i = 4; i<8; i++)
						{
							 uint8_t CurrentByte = ( DataLsb >> ((7-i) * 8) );
							 switch (i)
							 {						
								case 4:
									KartPosMsbY[2] = CurrentByte;
									break;
								case 5:
									KartPositionY[2] = (KartPosMsbY[2] << 8) | CurrentByte;
									break;
								case 6:
									KartOriMsb[2] = CurrentByte;
								case 7:
									KartOrientation[2] = (KartOriMsb[2] << 8) | CurrentByte;
								default:
									break;
							 }
						 }
						
						 if (KartOrientation[2] < 0) KartOrientation[2] += 360;

						CurrentState = ST_DRS_IDLE;
					}
				 break;
			 }
    }
    return(ReturnEvent);
}

DrsState_t QueryDrsSM ( void )
{
   return(CurrentState);
}


uint8_t GetRemainingLaps( uint8_t KartNum )
{
	uint8_t result = KartStatus[KartNum] & (BIT0HI | BIT1HI | BIT2HI);
	return result;
}

uint8_t GetRaceStatus( uint8_t KartNum )
{
	uint8_t result = KartStatus[KartNum] & (BIT3HI | BIT4HI);
	result = result >> 3;		// right shift 3 bits
	return result;
}

bool IsTargetHit( uint8_t KartNum )
{
	return (KartStatus[KartNum] & BIT7HI);
}

bool IsObstacleDone( uint8_t KartNum )
{
	return (KartStatus[KartNum] & BIT6HI);
}

uint16_t GetKartPositionX( uint8_t KartNum )
{
	return KartPositionX[KartNum];
}
uint16_t GetKartPositionY( uint8_t KartNum )
{
	return KartPositionY[KartNum];
}
uint16_t GetKartOrientation( uint8_t KartNum )
{
	return KartOrientation[KartNum];
}

void AddU( int16_t value)
{
	U += value;
	if (U > 360)
	{
		U -= 360;
	}
	if (U < 0)
	{
		U += 360;
	}
}