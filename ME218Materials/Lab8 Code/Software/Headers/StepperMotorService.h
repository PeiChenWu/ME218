#ifndef STEPPERMOTORSERVICE_H
#define STEPPERMOTORSERVICE_H

#include "ES_Configure.h"
#include "ES_Types.h"
#include "ES_Framework.h"

// Public Function Prototypes

bool InitStepperMotorService( uint8_t Priority );
bool PostStepperMotorService( ES_Event ThisEvent );
ES_Event RunStepperMotorService( ES_Event ThisEvent );


#endif