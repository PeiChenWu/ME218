#ifndef DrsSM_H
#define DrsSM_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { ST_DRS_IDLE, ST_GAME_STATUS, ST_KART_1, ST_KART_2, ST_KART_3 } DrsState_t ;


// Public Function Prototypes

bool PostDrsSM( ES_Event ThisEvent );
bool InitDrsSM ( uint8_t Priority );
ES_Event RunDrsSM( ES_Event CurrentEvent );
DrsState_t QueryDrsSM ( void );

// Interface for the KartSM and EventChecker to query information

uint8_t GetRemainingLaps( uint8_t KartNum );
uint8_t GetRaceStatus( uint8_t KartNum );
bool IsTargetHit( uint8_t KartNum );
bool IsObstacleDone( uint8_t KartNum );

uint16_t GetKartPositionX( uint8_t KartNum );
uint16_t GetKartPositionY( uint8_t KartNum );
uint16_t GetKartOrientation( uint8_t KartNum );

void AddU( int16_t value);

#endif
