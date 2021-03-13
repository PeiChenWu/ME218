/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts

 ****************************************************************************/

#ifndef TopHSMTemplate_H
#define TopHSMTemplate_H

// State definitions for use with the query function
typedef enum { StateOne, StateTwo, StateThree } MasterState_t ;

// Public Function Prototypes

ES_Event RunMasterSM( ES_Event CurrentEvent );
void StartMasterSM ( ES_Event CurrentEvent );
boolean PostMasterSM( ES_Event ThisEvent );
boolean InitMasterSM ( uint8_t Priority );

#endif /*TopHSMTemplate_H */

