/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef TimeKeeping_H
#define TimeKeeping_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { STATE_DisplayTime, STATE_DisplayDate,
                STATE_Uninitialized } TimeKeepingState_t ;


// Public Function Prototypes

ES_Event RunTimeKeepingSM( ES_Event CurrentEvent );
void StartTimeKeepingSM ( ES_Event CurrentEvent );
TimeKeepingState_t QueryTimeKeepingSM ( void );

#endif /*TimeKeeping_H */

