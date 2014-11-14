#include "contiki.h"
#include "window.h"
#include "grlib/grlib.h"
#include "memlcd.h"
#include "rtc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint8_t month, now_month, day, now_day,today_week;
uint16_t year, now_year;



int getWeek() {
    uint16_t daysUpToday;
    uint8_t mes;
    uint8_t setmana;
    uint8_t DayOffset;
    uint8_t US_EU_adjust=0;

    ////////////////////////////////////////
    ////////// read all the months to get all the max days
    daysUpToday=0;
    if (window_readconfig()->date_format!=0)	//USA config add one day
       	  {
    			US_EU_adjust=1;
       	  }

    for(mes = 1; mes < now_month; mes++)
    {
    	daysUpToday+=rtc_getmaxday(year, mes);
    	//printf("days=>%i (%i,%i)\n",daysUpToday,mes,year);
    }



    DayOffset=rtc_getweekday(year, 1, 1) - 1-US_EU_adjust; // use 0 as index  Europe
    setmana=1+(daysUpToday+now_day+DayOffset)/7;

    printf("Dayoffset %d\n", DayOffset);

    if ((US_EU_adjust+DayOffset)>4) {	// For start week short ==>w53
    	setmana=setmana-1;
    	if (setmana==0) {setmana=53;}
    }

    printf("daysUpToday %i,DayOffset %i,now_day %i\n",daysUpToday,DayOffset,now_day);
    return(setmana);
}

static void OnDraw(tContext *pContext)
{
  char buf[20];

  // clear screen
  GrContextForegroundSet(pContext, ClrBlack);
  GrContextBackgroundSet(pContext, ClrWhite);
  GrRectFill(pContext, &fullscreen_clip);

  // draw table title
  GrContextForegroundSet(pContext, ClrWhite);
  GrContextBackgroundSet(pContext, ClrBlack);
  const tRectangle rect = {0, 27, 255, 41};
  GrRectFill(pContext, &rect);

  // draw the title bar
  GrContextFontSet(pContext, &g_sFontGothic18b);
  sprintf(buf, "%s %d", toMonthName(month, 1), year);
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 15, 0);

  GrContextForegroundSet(pContext, ClrBlack);
  GrContextBackgroundSet(pContext, ClrWhite);
  GrContextFontSet(pContext, &g_sFontGothic18);
  for(int i = 0; i < 7; i++)
  {
	  if (window_readconfig()->date_format==0)	//European config
	  {
		  GrStringDrawCentered( pContext, week_shortnameEU[i], -1, i * 20 + 11, 35, 0);
	  }
	  else										//US config
	  {
		  GrStringDrawCentered( pContext, week_shortnameUS[i], -1, i * 20 + 11, 35, 0);
	  }


  }

  GrContextFontSet(pContext, &g_sFontGothic18);
  GrContextForegroundSet(pContext, ClrWhite);
  GrContextBackgroundSet(pContext, ClrBlack);
  uint8_t weekday;
  // get the start point of this month
  	   //------------------------------------------------------------------------------------------
   	   // European - US format
  	  	  if (window_readconfig()->date_format==0)	//European config	//European config
  	  	  {
  	  	     weekday = rtc_getweekday(year, month, 1) - 2; // use 0 as index  // canviat 1 US
  	  	  }
  	  	  else
  	  	  {
  	  		 weekday = rtc_getweekday(year, month, 1) - 1; // use 0 as index  // original 1 US format
  	  	  }


  uint8_t maxday = rtc_getmaxday(year, month);
  uint8_t y = 50;
  //printf("inside rtc-getmaxday: month-year ==> %i - %i ==>%i\n",month,year,maxday);

  for(int day = 1; day <= maxday; day++)
  {
    sprintf(buf, "%d", day);

    uint8_t today = now_year == year && now_month == month && now_day == day;
    if (today)
    {
      const tRectangle rect = {weekday * 20 + 1, y - 7, 20 + weekday * 20 - 1, y + 7};
      GrRectFill(pContext, &rect);
      GrContextForegroundSet(pContext, ClrBlack);
      GrContextBackgroundSet(pContext, ClrWhite);
    }

    GrStringDrawCentered( pContext, buf, -1, weekday * 20 + 11, y, 0);
    if (today)
    {
      const tRectangle rect2 = {weekday * 20 + 16, y - 5, weekday * 20 + 17, y - 4};
      GrRectFill(pContext, &rect2);

      GrContextForegroundSet(pContext, ClrWhite);
      GrContextBackgroundSet(pContext, ClrBlack);
    }

   // if (weekday != 6)
     // GrLineDrawV(pContext, (weekday + 1 ) * 20, 42, y+20 + 7);

    weekday++;
    if (weekday == 7)
    {
     // GrLineDrawH(pContext, 0, LCD_WIDTH, y+20 + 8);

      weekday = 0;
      y += 20;
    }
  }

 // GrLineDrawH(pContext, 0, weekday * 20, y + 8);

  // draw the buttons
  if (month == 1)
    sprintf(buf, "%s %d", toMonthName(12, 0), year - 1);
  else
    sprintf(buf, "%s %d", toMonthName(month - 1, 0), year);
//  window_button(pContext, KEY_ENTER, buf);

  if (month == 12)
    sprintf(buf, "%s %d", toMonthName(1, 0), year + 1);
  else
    sprintf(buf, "%s %d", toMonthName(month + 1, 0), year);
 // window_button(pContext, KEY_DOWN, buf);

  today_week=getWeek();

  sprintf(buf, "Week: %d",today_week);


  //printf("week: %i",today_week);


  GrContextFontSet(pContext, &g_sFontGothic18);
  GrContextForegroundSet(pContext, ClrWhite);
  GrContextBackgroundSet(pContext, ClrBlack);
  GrStringDrawCentered( pContext, buf, -1, 110, 150, 0);

}


uint8_t calendar_process(uint8_t ev, uint16_t lparam, void* rparam)
{
  if (ev == EVENT_WINDOW_CREATED)
  {
    rtc_readdate(&year, &month, &day, NULL);
    now_month = month;
    now_year = year;
    now_day = day;
    return 0x80;
  }
  else if (ev == EVENT_KEY_PRESSED)
  {
    if (lparam == KEY_ENTER)
    {
      if (month == 1)
      {
        month = 12;
        year--;
      }
      else
      {
        month--;
      }
      window_invalid(NULL);
    }
    else if (lparam == KEY_DOWN)
    {
      if (month == 12)
      {
        month = 1;
        year++;
      }
      else
      {
        month++;
      }
      window_invalid(NULL);
    }
  }
  else if (ev == EVENT_WINDOW_PAINT)
  {
    OnDraw((tContext*)rparam);
  }
  else
  {
    return 0;
  }

  return 1;
}
