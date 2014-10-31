
#include "contiki.h"
#include "window.h"
#include "rtc.h"
#include "math.h"
#include "grlib/grlib.h"
#include "memlcd.h"
#include "memory.h"
#include <time.h>
#include "icons.h"

#include <stdio.h> // for sprintf
#include <string.h>

#define BATTERY_EMPTY 0
#define BATTERY_LESS  1
#define BATTERY_MORE  2
#define BATTERY_FULL  3
#define BATTERY_CHARGING 4

#define _hour0 d.digit.hour0
#define _minute d.digit.minute
#define _second d.digit.sec
static uint8_t _selection;
uint8_t _weekday;

extern uint8_t disable_key;
extern uint16_t status;
extern void check_battery();

typedef void (*draw_function)(tContext *pContext);

void adjustAMPM(uint8_t hour, uint8_t *outhour, uint8_t *ispm)
{
  if (hour > 12) // 12 - 23
  {
    *ispm = 1;
    *outhour -= 12;
  }
  else if (hour == 12)
  {
    *ispm = 1;
  }
  else if (hour == 0)
  {
    *outhour = 12;
  }
}

 static void drawTopIcons (tContext* pContext)
{
	check_battery();
		GrContextForegroundSet(pContext, ClrWhite);
		//GrLineDrawH(pContext, 0, LCD_WIDTH, 16);
		GrContextFontSet(pContext, (tFont*)&g_sFontExIcon16);
	    char icon;


	    switch(status & 0x03)
	    {
	      case BATTERY_EMPTY:
	        icon = ICON_BATTERY_EMPTY;
	        break;
	      case BATTERY_LESS:
	        icon = ICON_BATTERY_LESS;
	        break;
	      case BATTERY_MORE:
	        icon = ICON_BATTERY_MORE;
	        break;
	      case BATTERY_FULL:
	        icon = ICON_BATTERY_FULL;
	        break;
	      default:
	        icon = 0;
	    }

	    if (status & BATTERY_CHARGING)
	    {
	     // GrStringDraw(pContext, &icon, 1, 120, 0, 0);
	     // icon = ICON_CHARGING;
	      //GrStringDraw(pContext, &icon, 1, 137, 0, 0);
	    }
	    else
	    {
	      GrStringDraw(pContext, &icon, 1, 127, 0, 0);
	    }

}
static void drawClock0(tContext *pContext)
{
  uint16_t year;
  uint8_t hour = _hour0;
  uint8_t month, day;
  uint8_t ispm = 0;
  char buf[20];

  drawTopIcons(pContext);

  rtc_readdate(&year, &month, &day, NULL);

  // draw time
  adjustAMPM(hour, &hour, &ispm);

  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus38);

  sprintf(buf, "%02d:%02d", hour, _minute);
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH/2, 60, 0);

  int width = GrStringWidthGet(pContext, buf, -1);
  GrContextFontSet(pContext, &g_sFontGothic18);
  if (ispm) buf[0] = 'P';
    else buf[0] = 'A';
  buf[1] = 'M';
  GrStringDraw(pContext, buf, 2, LCD_WIDTH/2+width/2 - 
    GrStringWidthGet(pContext, buf, 2) - 2
    , 84, 0);

  GrContextFontSet(pContext, &g_sFontGothic18);
  sprintf(buf, "%s %d, %d", toMonthName(month, 1), day, year);
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH/2, 137, 0);
}

static void drawClock1(tContext *pContext)
{
  uint8_t ispm = 0;
  uint8_t hour = _hour0;
  char buf[20];

  drawTopIcons(pContext);
  // draw time
  adjustAMPM(hour, &hour, &ispm);

  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus52);

  sprintf(buf, "%02d", hour);
  GrStringDrawCentered(pContext, buf, 2, LCD_WIDTH/ 2, 60, 0);

  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus52);
  sprintf(buf, "%02d", _minute);
  GrStringDrawCentered(pContext, buf, 2, LCD_WIDTH / 2, 100, 0);

  GrContextFontSet(pContext, &g_sFontGothic18b);
  if (ispm) buf[0] = 'P';
    else buf[0] = 'A';
  buf[1] = 'M';
  GrStringDrawCentered(pContext, buf, 2, LCD_WIDTH / 2, 132, 0);
}

static void drawClock2(tContext *pContext)
{
  uint8_t ispm = 0;
  uint8_t hour = _hour0;
  char buf[20];
  const char* buffer;

  drawTopIcons(pContext);
  // draw time
  adjustAMPM(hour, &hour, &ispm);

  GrContextFontSet(pContext, &g_sFontNimbus34);
  buffer = toEnglish(hour, buf);
  GrStringDraw(pContext, buffer, -1, 5, 58, 0);

  GrContextFontSet(pContext, &g_sFontGothic18);
  buffer = toEnglish(_minute, buf);
  GrStringDraw(pContext, buffer, -1, 5, 88, 0);

  GrContextFontSet(pContext, &g_sFontGothic18);
  if (ispm)
  {
    GrStringDraw(pContext, "In the PM", -1, 5, 120, 0);
  }
  else
  {
    GrStringDraw(pContext, "In the AM", -1, 5, 120, 0);
  }
}

static void drawClock3(tContext *pContext)
{
  uint8_t ispm = 0;
  uint8_t hour = _hour0;
  char buf[20];
  const char* buffer;

  drawTopIcons(pContext);
  // draw time
  adjustAMPM(hour, &hour, &ispm);

  GrContextFontSet(pContext, &g_sFontNimbus34);

  buffer = toEnglish(hour, buf);
  tRectangle rect = {5, 32, LCD_WIDTH - 5, 80};
  GrRectFill(pContext, &rect);
  GrContextForegroundSet(pContext, ClrBlack);
  GrStringDrawCentered(pContext, buffer, -1, LCD_WIDTH/ 2, 56, 0);
  GrContextForegroundSet(pContext, ClrWhite);
  
  GrContextFontSet(pContext, &g_sFontGothic24b);
  buffer = toEnglish(_minute, buf);
  GrStringDrawCentered(pContext, buffer, -1, LCD_WIDTH / 2, 95, 0);

  GrContextFontSet(pContext, &g_sFontGothic18);
  if (ispm)
  {
    GrStringDrawCentered(pContext, "In the PM", -1, LCD_WIDTH / 2, 120, 0);
  }
  else
  {
    GrStringDrawCentered(pContext, "In the AM", -1, LCD_WIDTH / 2, 120, 0);
  }
}

static void drawClock8(tContext *pContext)
{
    GrContextForegroundSet(pContext, ClrWhite);
    GrRectFill(pContext, &fullscreen_clip);
    GrContextForegroundSet(pContext, ClrBlack);

    drawClock0(pContext);
}

static void drawClock4(tContext *pContext)
{
  uint16_t year;
  uint8_t hour = _hour0;
  uint8_t month, day;
  char buf[20];
  drawTopIcons(pContext);
  rtc_readdate(&year, &month, &day, NULL);

  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus50);

  sprintf(buf, "%02d:%02d", hour, _minute);
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 68, 0);

  GrContextFontSet(pContext, &g_sFontGothic24b);



  //------------------------------------------------------------------------------------------
  // European - US format
  	  	  if (window_readconfig()->date_format==0)	//European config	//European config
		  {
			  sprintf(buf, "%d %s, %d",day, toMonthName(month, 1), year);
		  }
		  else																//US config
		  {
			  sprintf(buf, "%s %d, %d", toMonthName(month, 1), day, year);
		  }
  //------------------------------------------------------------------------------------------



  tRectangle rect = {6, 90, LCD_WIDTH-6, 120};
  GrRectFillRound(pContext, &rect, 8);
  GrContextForegroundSet(pContext, ClrBlack);
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 105, 0);
}

static void drawClock5(tContext *pContext)
{
  uint16_t year;
  uint8_t hour = _hour0;
  uint8_t month, day;
  uint8_t ispm = 0;
  char buf[20];
  drawTopIcons(pContext);
  rtc_readdate(&year, &month, &day, NULL);

  // draw time
  adjustAMPM(hour, &hour, &ispm);

  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus46);

  sprintf(buf, "%02d:%02d", hour, _minute);  
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 70, 0);

  int width = GrStringWidthGet(pContext, buf, -1);
  GrContextFontSet(pContext, &g_sFontGothic24b);
  if (ispm) buf[0] = 'P';
    else buf[0] = 'A';
  buf[1] = 'M';
  GrStringDraw(pContext, buf, 2, LCD_WIDTH/2+width/2 - 
    GrStringWidthGet(pContext, buf, 2) - 2
    , 92, 0);

  GrContextFontSet(pContext, &g_sFontGothic18);

  //------------------------------------------------------------------------------------------
   // European - US format
  	  	  if (window_readconfig()->date_format==0)	//European config	//European config
 		  {
 			 sprintf(buf, "%02d-%02d-%02d", day,month, year - 2000);
 		  }
 		  else																//US config
 		  {
 			 sprintf(buf, "%02d-%02d-%02d", month,day, year - 2000);
 		  }
   //------------------------------------------------------------------------------------------





  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 143, 0);
}

static void drawClock6(tContext *pContext)
{
  uint16_t year;
  uint8_t hour = _hour0;
  uint8_t month, day;
  char buf[20];
  drawTopIcons(pContext);
  rtc_readdate(&year, &month, &day, NULL);

  tRectangle rect = {30, 26, LCD_WIDTH - 30, LCD_Y_SIZE - 37};
  GrRectFillRound(pContext, &rect, 8);
  GrContextForegroundSet(pContext, ClrBlack);
  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus52);
  sprintf(buf, "%02d", hour);
  GrStringDrawCentered(pContext, buf, 1, LCD_WIDTH / 4 + 20, 54, 0);
  GrStringDrawCentered(pContext, buf + 1, 1, LCD_WIDTH / 2 + LCD_WIDTH/4 - 20, 54, 0);

  sprintf(buf, "%02d", _minute);
  GrStringDrawCentered(pContext, buf, 1, LCD_WIDTH / 4 + 20, 100, 0);
  GrStringDrawCentered(pContext, buf + 1, 1, LCD_WIDTH / 2 + LCD_WIDTH/4 - 20, 100, 0);

  GrContextFontSet(pContext, &g_sFontGothic24b);
  GrContextForegroundSet(pContext, ClrWhite);

  //------------------------------------------------------------------------------------------
   // European - US format
  	  	  if (window_readconfig()->date_format==0)	//European config	//European config
 		  {
 			  sprintf(buf, "%d-%d-%d", day, month, year);
 		  }
 		  else																//US config
 		  {
 			  sprintf(buf, "%d-%d-%d", month, day, year);
 		  }
   //------------------------------------------------------------------------------------------





  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 150, 0);
}

static void drawClock7(tContext *pContext)
{
  uint16_t year;
  uint8_t hour = _hour0;
  uint8_t month, day;
  uint8_t ispm = 0;
  char buf[20];
  drawTopIcons(pContext);
  rtc_readdate(&year, &month, &day, NULL);

  // draw time
  adjustAMPM(hour, &hour, &ispm);

  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus52);

  sprintf(buf, "%02d:%02d", hour, _minute);
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 70, 0);

  int width = GrStringWidthGet(pContext, buf, -1);
  GrContextFontSet(pContext, &g_sFontGothic18b);
  if (ispm) buf[0] = 'P';
    else buf[0] = 'A';
  buf[1] = 'M';
  GrStringDraw(pContext, buf, 2, LCD_WIDTH/2+width/2 - 
    GrStringWidthGet(pContext, buf, 2) - 2
    , 95, 0);


  GrContextFontSet(pContext, &g_sFontGothic24b);

  //------------------------------------------------------------------------------------------
  // European - US format
  	  	  if (window_readconfig()->date_format==0)	//European config
		  {
			  sprintf(buf, "%d %s, %d", day, toMonthName(month, 1), year);
		  }
		  else																//US config
		  {
			  sprintf(buf, "%s %d, %d", toMonthName(month, 1), day, year);
		  }
  //------------------------------------------------------------------------------------------




  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 35, 0);
}

static void drawClock9(tContext *pContext)
{
  uint16_t year;
  uint8_t hour = _hour0;
  uint8_t month, day;
  char buf[20];
  drawTopIcons(pContext);
  /*	  int julian = getDayOfYear(myDate)  // Jan 1 = 1, Jan 2 = 2, etc...
      int dow = _weekday     // Sun = 0, Mon = 1, etc...
      int dowJan1 = getDayOfWeek("1/1/" + thisYear)   // find out first of year's day
      int badWeekNum = (julian / 7) + 1  // Get our week# (wrong)
      int weekNum = ((julian + 6) / 7)   // probably better.  CHECK THIS LINE. (See comments.)
      if (dow < dowJan1)                 // adjust for being after Saturday of week #1
          ++weekNum;
      return (weekNum)
  */

  	time_t rawtime;
    struct tm * timeinfo;


    time (&rawtime);
    timeinfo = localtime (&rawtime);





  rtc_readdate(&year, &month, &day, &_weekday);

  // draw time
  GrContextFontSet(pContext, (tFont*)&g_sFontExNimbus91);

  sprintf(buf, "%02d", hour);
  GrStringDrawCentered(pContext, buf, 1, LCD_WIDTH / 4 + 10, 43, 0);
  GrStringDrawCentered(pContext, buf + 1, 1, LCD_WIDTH / 2 + LCD_WIDTH / 4 - 10, 43, 0);

  sprintf(buf, "%02d", _minute);
  GrStringDrawCentered(pContext, buf, 1, LCD_WIDTH / 4 + 10, 110, 0);
  GrStringDrawCentered(pContext, buf + 1, 1, LCD_WIDTH / 2 + LCD_WIDTH/4 - 10, 110, 0);

  GrContextFontSet(pContext, &g_sFontGothic24b);
  //------------------------------------------------------------------------------------------
  // European - US format
  	  	  printf("window_readconfig()->date_format=%d\n",window_readconfig()->date_format);
  	  	  printf("date_format_short[window_readconfig()->date_format]=%s\n",date_format_short[window_readconfig()->date_format]);

  	  	  if (window_readconfig()->date_format==0)	//European config
		  {
			  sprintf(buf, "%d-%d-%d", day, month, year);
		  }
		  else																//US config
		  {
			  sprintf(buf, "%d-%d-%d", month, day, year);
		  }
  //------------------------------------------------------------------------------------------



  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH / 2, 155, 0);

/*  sprintf(buf, "W");
  GrStringDrawCentered(pContext, buf, -1, 12, 135, 0);


  strftime (buf,3,"%V",timeinfo);
  GrStringDrawCentered(pContext, buf, -1, 12, 155, 0);
*/

  sprintf(buf, "%02d",_second);
  GrStringDrawCentered(pContext, buf, -1, LCD_WIDTH-12, 131, 0);
}

static const draw_function Clock_selections[] =
{
  drawClock0,
  drawClock1,
  drawClock2,
  drawClock3,
  drawClock4,
  drawClock5,
  drawClock6,
  drawClock7,
  drawClock8,
  drawClock9,
};

uint8_t digitclock_process(uint8_t ev, uint16_t lparam, void* rparam)
{
  if (ev == EVENT_WINDOW_CREATED)
  {
    rtc_readtime(&_hour0, &_minute, &_second);
    if (rparam == NULL)
      _selection = window_readconfig()->digit_clock;
    else
      _selection = (uint8_t)rparam - 1;
    rtc_enablechange(SECOND_CHANGE);

    return 0x80;
  }
  else if (ev == EVENT_WINDOW_ACTIVE)
  {
    rtc_readtime(&_hour0, &_minute, &_second);
  }
  else if (ev == EVENT_WINDOW_PAINT)
  {
    // clear the region
    tContext *pContext = (tContext*)rparam;
    GrContextForegroundSet(pContext, ClrBlack);
    GrRectFill(pContext, &fullscreen_clip);
    GrContextForegroundSet(pContext, ClrWhite);

    Clock_selections[_selection](pContext);
  }
  else if (ev == EVENT_TIME_CHANGED)
  {
    struct datetime* dt = (struct datetime*)rparam;
    _hour0 = dt->hour;
    _minute = dt->minute;
    _second= dt->second;






    window_invalid(NULL);


  }
  else if ((ev == EVENT_KEY_PRESSED) && (disable_key == 0))
  {
    if (lparam == KEY_DOWN)
    {
      _selection += 0x1;
      if (_selection > sizeof(Clock_selections)/sizeof(draw_function) - 1)
      {
        _selection = 0x00;
      }
      window_invalid(NULL);
    }
    else if (lparam == KEY_UP)
    {
      _selection -= 0x1;
      if (_selection == 0xff)
      {
        _selection = sizeof(Clock_selections)/sizeof(draw_function) - 1;
      }
      window_invalid(NULL);
    }
  }

  else if ((ev == EVENT_KEY_PRESSED) && (disable_key != 0))
   {
	  if (lparam == KEY_DOWN){

		  window_open(&calendar_process, NULL);
	  }
   }

  else if (ev == EVENT_WINDOW_CLOSING)
  {
    rtc_enablechange(0);

    window_readconfig()->default_clock = 9;
    if (_selection != window_readconfig()->digit_clock)
    {
      window_readconfig()->digit_clock = _selection;
      window_writeconfig();
    }
  }
  else
  {
    return 0;
  }

  return 1;
}
