#ifndef BeaconSensingService_H
#define BeaconSensingService_H

#include <stdint.h>
#include <stdbool.h>

// typedefs for the states
// State definitions for use with the query function
typedef enum { ST_RESET, ST_READ } BeaconState_t ;

// Public Function Prototypes
bool PostBeaconSensingService( ES_Event ThisEvent );
bool InitBeaconSensingService ( uint8_t Priority );
ES_Event RunBeaconSensingService( ES_Event CurrentEvent );
BeaconState_t QueryBeaconSensingService ( void );

// Interface for the KartSM and EventChecker to query information
uint32_t GetBeaconValue(void);

#endif

