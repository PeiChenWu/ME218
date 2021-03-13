/***********************************************************************
    FILE : PWMS12.h -- public header for pwms12.c
    CREATED : June 8, 2004
    REVISED :
***********************************************************************/


#ifndef PWMS12_H
#define PWMS12_H


	/* === DEFINITIONS === */
	/* --- Channels --- */
#define PWMS12_CHAN0 0
#define PWMS12_CHAN1 1
#define PWMS12_CHAN2 2

#define PWMS12_GRP0 0
#define PWMS12_GRP1 1

#define PWMS12_ERR     -1
#define PWMS12_OK       0

#define PWMS12_MIN_DC   0
#define PWMS12_MAX_DC   100

    /* --- Period Constants --- */

#define PWMS12_4000US 20510
#define PWMS12_3300US 8390
#define PWMS12_3300US 8342
#define PWMS12_2000US 16414
#define PWMS12_1950US 4330
#define PWMS12_1650US 4294
#define PWMS12_1250US 4246
#define PWMS12_1000US 12318
#define PWMS12_825US 198
#define PWMS12_650US 4174
#define PWMS12_500US 8222




	/* === FUNCTION PROTOTYPES === */
void PWMS12_Init(void);
signed char PWMS12_SetDuty(unsigned char dutyCycle, unsigned char channel);
signed char PWMS12_SetPeriod(unsigned short newPeriod, unsigned char group);
void PWMS12_End(void);

#endif /* S12PWM_H */

