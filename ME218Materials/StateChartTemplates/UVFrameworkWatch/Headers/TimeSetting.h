/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef TimeSetting_H
#define TimeSetting_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { STATE_SetHour, STATE_SetMin,
                STATE_SetSec, STATE_SetDay, STATE_SetMonth,
                STATE_SetYear} TimeSettingState_t ;


// Public Function Prototypes

ES_Event RunTimeSettingSM( ES_Event CurrentEvent );
void StartTimeSettingSM ( ES_Event CurrentEvent );
TimeSettingState_t QueryTimeSettingSM ( void );

#endif /*TimeKeeping_H */

