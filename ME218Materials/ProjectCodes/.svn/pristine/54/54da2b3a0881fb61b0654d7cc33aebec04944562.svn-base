#ifndef ShootingSM_H
#define ShootingSM_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { ST_ToShootingRegion,
							 ST_AlignBeacon, 
							 ST_Loading, 
							 ST_Shooting,
							 ST_Shooting_Idle,
							 ST_Navigating
						 } ShootingState_t ;



// Public Function Prototypes

ES_Event RunShootingSM( ES_Event CurrentEvent );
void StartShootingSM ( ES_Event CurrentEvent );
ShootingState_t ShootingSM ( void );

#endif
