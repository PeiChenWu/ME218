// the routines to keep the time variables and display the time or date on 
// command

#include <stdint.h>
#include <stdio.h>

#include "TimeDisplay.h"

static uint8_t Day=1;
static uint8_t Month=1;
static uint16_t Year=1970;

static uint8_t Hour=11;
static uint8_t Min=59;
static uint8_t Sec=0;

static bool AM = true;

bool IncrementSeconds( void )
{
  if (++Sec > 59)
  {  
    Sec = 0;
    return true;
  }else{
    return false;
  }
}

bool IncrementMinutes( void )
{
  if (++Min > 59)
  {  
    Min = 0;
    return true;
  }else{
    return false;
  }
}

bool IncrementHours( void )
{
  if (++Hour > 12)
  {  
    Hour = 1;
  }else{
    if (Hour > 11){
      AM = !AM;
      return true;
    }
  }
  return false;
}

bool IncrementDays( void )
{
  uint8_t DayLimit;
  
  switch (Month){
    case 9:
    case 4:
    case 6:
    case 11:
      DayLimit = 30;
      break;
    
    case 2:
      DayLimit = 28; // we are not going to hande leap years
      break;
    
    default:
      DayLimit = 31;
      break;
  }
  
  if (++Day > DayLimit){
    Day = 1;
    return true;
  }else{
    return false;
  }
}

bool IncrementMonths( void )
{
  if (++Month > 12)
  {
    Month = 1;
    return true;
  }else{
    return false;
  }
}

void IncrementYears( void )
{
  ++Year; 
}

void TickClock( void )
{
  if (true == IncrementSeconds())
    if(true == IncrementMinutes())
      if(true == IncrementHours())
        if(true == IncrementDays())
          if(true == IncrementMonths())
            IncrementYears();
}
void DisplayTime(void)
{
  printf("%02d:%02d:%02d",Hour,Min,Sec);
  if (true == AM)
    printf("AM\r");
  else
    printf("PM\r");
}
void DisplayDate(void)
{
  printf("%02d/%02d/%4d\r",Month,Day,Year);
}
void DisplayHour(void)
{
  printf("Hour: %02d  \r",Hour);
}
void DisplayMinute(void)
{
  printf("Minutes:%02d\r",Min);
}
void DisplaySeconds(void)
{
  printf("Seconds:%02d\r",Sec);
}
void DisplayDay(void)
{
  printf("Day: %02d   \r",Day);
}
void DisplayMonth(void)
{
  printf("Month: %02d \r",Month);
}
void DisplayYear(void)
{
  printf("Year: %4d\r",Year);
}
