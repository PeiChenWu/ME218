/*****************************************************************************************************************
This is the header file for all initialization functions for ME218b project.
This file includes macro definition for all pins used in the project.
*****************************************************************************************************************/

#ifndef INIT_H
#define INIT_H

/*****************************************************************************************************************/
// the headers to access the GPIO subsystem
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#define ALL_BITS (0xff<<2)
/*****************************************************************************************************************/



/*****************************************************************************************************************/
//Interface for initialize all required pins and alternate functions
void InitAll(void);

//Global varieble
extern uint8_t MyKartNum;
/*****************************************************************************************************************/




/*****************************************************************************************************************/
// Pins & Ports definition for Sensors

// Analog Input Pins will be initialized in ADMulti Library
// Beacon Sensing - PB5 - Reset Control for Peak Detection
#define BEACON_PORT GPIO_PORTB_BASE
#define BEACON_DATA BEACON_PORT+(GPIO_O_DATA + ALL_BITS)
#define BEACON_PIN	GPIO_PIN_5

// Ultrasonic Receiver - PE0 - AIN0
//#define ULTRASOUND_PORT GPIO_PORTE_BASE+(GPIO_O_DATA + ALL_BITS)
//#define ULTRASOUND_PIN 	GPIO_PIN_0

// DIP Switch - PB0 & PB1 - Input Pullup
#define	DIPSW_PORT GPIO_PORTB_BASE
#define DIPSW_DATA GPIO_PORTB_BASE+(GPIO_O_DATA + ALL_BITS)
#define DIPSW_PINS (GPIO_PIN_0 | GPIO_PIN_1)
/*****************************************************************************************************************/



/*****************************************************************************************************************/
// Pins & Ports definition for KART locomotion

// Left Motor Control (Direction, PWM, Brake)  - PD2, PD0, PD6
// Right Motor Control (Direction, PWM, Brake) - PD3, PD1, PD7

#define MOTOR_PORT GPIO_PORTD_BASE
#define MOTOR_DATA GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)

#define LEFT_MOTOR_DIR_PIN 		 GPIO_PIN_2
#define LEFT_MOTOR_PWM_PIN		 GPIO_PIN_0 
#define LEFT_MOTOR_BRAKE_PIN   GPIO_PIN_6
#define RIGHT_MOTOR_DIR_PIN		 GPIO_PIN_3
#define RIGHT_MOTOR_PWM_PIN		 GPIO_PIN_1
#define RIGHT_MOTOR_BRAKE_PIN  GPIO_PIN_7

// Left Motor Encoder Input  - PC6 - WT1CCP0 Input Capture
// Right Motor Encoder Input - PC7 - WT1CCP1 Input Capture
#define MOTOR_ENCODER_PORT GPIO_PORTC_BASE
#define MOTOR_ENCODER_DATA GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)
#define LEFT_MOTOR_ENCODER_PIN  GPIO_PIN_6
#define RIGHT_MOTOR_ENCODER_PIN GPIO_PIN_7
/*****************************************************************************************************************/



/*****************************************************************************************************************/
// Pins & Ports definition for Shooting Mechanism

// Pitching Motor Enable - PB4 - M0PWM2
#define PITCH_MOTOR_PORT GPIO_PORTB_BASE
#define PITCH_MOTOR_DATA GPIO_PORTB_BASE+(GPIO_O_DATA + ALL_BITS)
#define PITCH_MOTOR_PIN  GPIO_PIN_4

// Latch 1 Servo - PE4 - M0PWM4
// Latch 2 Servo - PE5 - M0PWM5
#define LATCH_PORT  GPIO_PORTE_BASE
#define LATCH_1_PIN GPIO_PIN_4
#define LATCH_2_PIN GPIO_PIN_5
/*****************************************************************************************************************/


/*****************************************************************************************************************/
// Pins & Ports definition for Indicators

// Game Status LED Green - PB2
// Game Status LED Red   - PB3
#define GAME_LED_PORT GPIO_PORTB_BASE
#define GAME_LED_DATA GAME_LED_PORT+(GPIO_O_DATA + ALL_BITS)
#define GAME_LED_GREEN_PIN GPIO_PIN_2
#define GAME_LED_RED_PIN GPIO_PIN_3
/*****************************************************************************************************************/

#endif
