#ifndef KartSM_H
#define KartSM_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { ST_RACING, ST_SHOOTING, ST_OBS_CROSSING } KartState_t ;


// Public Function Prototypes

bool PostKartSM( ES_Event ThisEvent );
bool InitKartSM ( uint8_t Priority );
ES_Event RunKartSM( ES_Event CurrentEvent );
void StartKartSM ( ES_Event CurrentEvent );
KartState_t QueryKartSM ( void );

#endif
