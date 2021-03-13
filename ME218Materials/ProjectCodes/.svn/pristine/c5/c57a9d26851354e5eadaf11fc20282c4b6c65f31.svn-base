#ifndef KARTMasterSM_H
#define KARTMasterSM_H

// State definitions for use with the query function
typedef enum { Racing, Shooting, ObstacleCrossing } MasterState_t ;

// Public Function Prototypes

ES_Event RunMasterSM( ES_Event CurrentEvent );
void StartMasterSM ( ES_Event CurrentEvent );
bool PostMasterSM( ES_Event ThisEvent );
bool InitMasterSM ( uint8_t Priority );

#endif