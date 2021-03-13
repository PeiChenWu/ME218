#ifndef DCMOTORSM_H
#define DCMOTORSM_H

#include "ES_Configure.h"
#include "ES_Types.h"
#include "ES_Framework.h"

// Public Function Prototypes

// typedefs for the states
// State definitions for use with the query function
typedef enum	{ initState= 0,
							  alignBeacon,
								detectTape,
								executeCmd}pulseState_t;

bool InitDCMotorSM( uint8_t Priority );
bool PostDCMotorSM( ES_Event ThisEvent );
ES_Event RunDCMotorSM( ES_Event ThisEvent );

#endif