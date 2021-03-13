/****************************************************************************
 Copyright (C) Stanford University 2010.
 Permission granted to freely use this file as long as this header remains
 intact.

 Description
         servolib.h is the header file for the R/C servo library.
         This header should be included by all applications programs wishing
         to call the servo library routines.
 Notes


 History
 When           Who	What/Why
 -------------- ---	--------
 11/04/10 19:40 jec updated for new SetPulseWidth function
 11/04/10 18:45 jec First pass
****************************************************************************/

#ifndef SERVO12_H
#define SERVO12_H

typedef enum  {  SERVO12_Err,
                 SERVO12_OK
} Servo12ReturnTyp;

/*
   function prototypes for the library
*/
Servo12ReturnTyp Servo12_Init( char * modeString );
Servo12ReturnTyp Servo12_SetPulseWidth( unsigned char ChannelNum, 
                                        unsigned int NewWidth );

#endif /* SERVO12_H */

                                                               