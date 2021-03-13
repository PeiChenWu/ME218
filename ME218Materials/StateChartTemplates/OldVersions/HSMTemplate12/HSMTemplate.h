/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef HSMTemplate_H
#define HSMTemplate_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { STATE_UNO, STATE_TWO, STATE_THREE } TemplateState_t ;


// Public Function Prototypes

ES_Event RunTemplateSM( ES_Event CurrentEvent );
void StartTemplateSM ( ES_Event CurrentEvent );
TemplateState_t QueryTemplateSM ( void );

#endif /*SHMTemplate_H */

