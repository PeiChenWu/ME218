/****************************************************************************
 Module
     ES_Configure.h
 Description
     This file contains macro definitions that are edited by the user to
     adapt the Events and Services framework to a particular application.
 Notes
     
 History
 When           Who     What/Why
 -------------- ---     --------
 10/21/13 20:54 jec      lots of added entries to bring the number of timers
                         and services up to 16 each
 08/06/13 14:10 jec      removed PostKeyFunc stuff since we are moving that
                         functionality out of the framework and putting it
                         explicitly into the event checking functions
 01/15/12 10:03 jec      started coding
*****************************************************************************/

#ifndef CONFIGURE_H
#define CONFIGURE_H

/****************************************************************************/
// The maximum number of services sets an upper bound on the number of 
// services that the framework will handle. Reasonable values are 8 and 16
// corresponding to an 8-bit(uint8_t) and 16-bit(uint16_t) Ready variable size
#define MAX_NUM_SERVICES 16

/****************************************************************************/
// This macro determines that nuber of services that are *actually* used in
// a particular application. It will vary in value from 1 to MAX_NUM_SERVICES
#define NUM_SERVICES 3

/****************************************************************************/
// These are the definitions for Service 0, the lowest priority service.
// Every Events and Services application must have a Service 0. Further 
// services are added in numeric sequence (1,2,3,...) with increasing 
// priorities
// the header file with the public function prototypes
#define SERV_0_HEADER "DrsSM.h"
// the name of the Init function
#define SERV_0_INIT InitDrsSM
// the name of the run function
#define SERV_0_RUN RunDrsSM
// How big should this services Queue be?
#define SERV_0_QUEUE_SIZE 3

/****************************************************************************/
// The following sections are used to define the parameters for each of the
// services. You only need to fill out as many as the number of services 
// defined by NUM_SERVICES
/****************************************************************************/
// These are the definitions for Service 1
#if NUM_SERVICES > 1
// the header file with the public function prototypes
#define SERV_1_HEADER "KartSM.h"
// the name of the Init function
#define SERV_1_INIT InitKartSM
// the name of the run function
#define SERV_1_RUN RunKartSM
// How big should this services Queue be?
#define SERV_1_QUEUE_SIZE 3
#endif


/****************************************************************************/
// These are the definitions for Service 3
#if NUM_SERVICES > 2
// the header file with the public function prototypes
#define SERV_2_HEADER "BeaconSensingService.h"
// the name of the Init function
#define SERV_2_INIT InitBeaconSensingService
// the name of the run function
#define SERV_2_RUN RunBeaconSensingService
// How big should this services Queue be?
#define SERV_2_QUEUE_SIZE 3
#endif


// Name/define the events of interest
// Universal events occupy the lowest entries, followed by user-defined events
typedef enum {  ES_NO_EVENT = 0,
                ES_ERROR,  /* used to indicate an error from the service */
                ES_INIT,   /* used to transition from initial pseudo-state */
                ES_NEW_KEY, /* signals a new key received from terminal */
                ES_TIMEOUT, /* signals that the timer has expired */
                ES_ENTRY,
                ES_ENTRY_HISTORY,
                ES_EXIT,
                /* events from template */
                ES_LOCK,
                ES_UNLOCK,
								/* events for our project */
								/* events for game status */
								EV_GAME_START,
							  EV_GAME_PAUSE,
							  EV_GAME_OVER,
								EV_SHOOTING,
								EV_OBS_CROSSING,
								/* events for navigation */
								EV_ARRIVE_R1,
								EV_ARRIVE_R2,
								EV_ARRIVE_R3,
								EV_ARRIVE_R4,
								EV_ARRIVE_SZONE,
								EV_ROTATE_DONE,
								/* event for encoders*/
								EV_ENCODER_GOAL,
								/*events for shooting*/
								EV_ALIGNED
							} ES_EventTyp_t ;

/****************************************************************************/
// These are the definitions for the Distribution lists. Each definition
// should be a comma separated list of post functions to indicate which
// services are on that distribution list.
#define NUM_DIST_LISTS 0
#if NUM_DIST_LISTS > 0 
#define DIST_LIST0 PostKartSM
#endif
#if NUM_DIST_LISTS > 1 
#define DIST_LIST1 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 2 
#define DIST_LIST2 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 3 
#define DIST_LIST3 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 4 
#define DIST_LIST4 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 5 
#define DIST_LIST5 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 6 
#define DIST_LIST6 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 7 
#define DIST_LIST7 PostTemplateFSM
#endif

/****************************************************************************/
// This are the name of the Event checking funcion header file. 
#define EVENT_CHECK_HEADER "EventCheckers.h"

/****************************************************************************/
// This is the list of event checking functions 
#define EVENT_CHECK_LIST Check4EncoderCount, Check4RaceStatusChange

/****************************************************************************/
// These are the definitions for the post functions to be executed when the
// corresponding timer expires. All 16 must be defined. If you are not using
// a timer, then you should use TIMER_UNUSED
// Unlike services, any combination of timers may be used and there is no
// priority in servicing them
#define TIMER_UNUSED ((pPostFunc)0)
#define TIMER0_RESP_FUNC TIMER_UNUSED
#define TIMER1_RESP_FUNC TIMER_UNUSED
#define TIMER2_RESP_FUNC TIMER_UNUSED
#define TIMER3_RESP_FUNC TIMER_UNUSED
#define TIMER4_RESP_FUNC TIMER_UNUSED
#define TIMER5_RESP_FUNC TIMER_UNUSED
#define TIMER6_RESP_FUNC TIMER_UNUSED
#define TIMER7_RESP_FUNC TIMER_UNUSED
#define TIMER8_RESP_FUNC TIMER_UNUSED
#define TIMER9_RESP_FUNC TIMER_UNUSED
#define TIMER10_RESP_FUNC TIMER_UNUSED
#define TIMER11_RESP_FUNC TIMER_UNUSED
#define TIMER12_RESP_FUNC PostBeaconSensingService
#define TIMER13_RESP_FUNC PostBeaconSensingService
#define TIMER14_RESP_FUNC PostDrsSM
#define TIMER15_RESP_FUNC PostDrsSM

/****************************************************************************/
// Give the timer numbers symbolc names to make it easier to move them
// to different timers if the need arises. Keep these definitions close to the
// definitions for the response functions to make it easier to check that
// the timer number matches where the timer event will be routed
// These symbolic names should be changed to be relevant to your application 

#define DRS_TIMER_SAMPLE 15
#define DRS_TIMER_QUERY 14
#define BEACON_TIMER_SAMPLE 13
#define BEACON_TIMER_RESET 13

#endif /* CONFIGURE_H */
