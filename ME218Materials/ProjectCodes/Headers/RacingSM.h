#ifndef RacingSM_H
#define RacingSM_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { ST_Racing_Idle,
							 ST_Navigating, 
							 ST_CheckGameStatus, 
							 ST_CheckObstacleStatus
						 } RacingState_t ;



// Public Function Prototypes

ES_Event RunRacingSM( ES_Event CurrentEvent );
void StartRacingSM ( ES_Event CurrentEvent );
RacingState_t QueryRacingSM ( void );

#endif
