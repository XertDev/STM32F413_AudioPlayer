#include "SetDate.hpp"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include "Utils.hpp"

extern RTC_HandleTypeDef hrtc;
extern LPTIM_HandleTypeDef hlptim1;

RTC_DateTypeDef date;
char date_buffer[11];
RTC_TimeTypeDef time_b;

constexpr Color background = from_r8g8b8(238, 244, 237);
constexpr Color back_button_color = from_r8g8b8(255, 0, 0);
constexpr Color save_button_color = from_r8g8b8(0, 255, 0);
constexpr Color navigation_color = from_r8g8b8(0, 0, 120);
constexpr Color separator_color = from_r8g8b8(0, 0, 0);
constexpr Color bar_color = from_r8g8b8(150, 150, 150);
constexpr uint8_t preferred_backlight = 10;

extern bool detected_touch;
extern bool timeout_lptim;

static void saveDate();
static void changeDay(int sign, LCDDisplay& display);
static void changeMonth(int sign, LCDDisplay& display);
static void changeYear(int sign, LCDDisplay& display);
static void draw_background(LCDDisplay& display);

void setDate(uint8_t* modes_stack, PeripheralsPack& pack) {
	draw_background(pack.lcd_display);
	pack.lcd_display.setBackgroundColor(background);

	pack.lcd_display.setTextColor(0x19AA);
	for(uint8_t i = pack.lcd_display.backlight(); i <= preferred_backlight; ++i) {
		pack.lcd_display.setBacklight(i);
		HAL_Delay(5);
	}

	HAL_RTC_GetTime(&hrtc, &time_b, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	sprintf(date_buffer, "%2d.%02d.%02d r.", date.Date, date.Month, date.Year);
	pack.lcd_display.drawString(35, 75, date_buffer);

	bool jump = false;

	while(true)
	{
		if(HAL_LPTIM_Counter_Start_IT(&hlptim1, 15360) != HAL_OK) {
			Error_Handler();
		}
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(detected_touch)
		{
			HAL_LPTIM_Counter_Stop_IT(&hlptim1);
			if(pack.touch_panel.detectTouch() == 1)
			{
				auto touch_details = pack.touch_panel.getDetails(0);
				if(touch_details.event_type == 1)
				{
					auto touch_info = pack.touch_panel.getPoint(0);
					if(inRange(touch_info.x, 0, 65) && inRange(touch_info.y, 200, 240))
					{
						uint8_t* last = modes_stack;
						while(*last != 0)
						{
							++last;
						}
						--last;
						*last = 3;
						jump = true;
					} else if(inRange(touch_info.x, 0, 65) && inRange(touch_info.y, 0, 40))
					{
						saveDate();
						uint8_t* last = modes_stack;
						while(*last != 0)
						{
							++last;
						}
						--last;
						*last = 3;
						jump = true;
					} else if(inRange(touch_info.x, 100, 140) && inRange(touch_info.y, 120, 240))
					{
						changeDay(1, pack.lcd_display);
					} else if(inRange(touch_info.x, 100, 140) && inRange(touch_info.y, 0, 120))
					{
						changeDay(-1, pack.lcd_display);
					} else if(inRange(touch_info.x, 150, 190) && inRange(touch_info.y, 120, 240))
					{
						changeMonth(1, pack.lcd_display);
					} else if(inRange(touch_info.x, 150, 190) && inRange(touch_info.y, 0, 120))
					{
						changeMonth(-1, pack.lcd_display);
					} else if(inRange(touch_info.x, 200, 240) && inRange(touch_info.y, 120, 240))
					{
						changeYear(1, pack.lcd_display);
					} else if(inRange(touch_info.x, 200, 240) && inRange(touch_info.y, 0, 120))
					{
						changeYear(-1, pack.lcd_display);
					}
				}
			}
		}

		if(timeout_lptim && !jump)
		{
			uint8_t* last = modes_stack;
			while(*last != 0)
			{
				++last;
			}
			*last = 2;
			timeout_lptim = 0;
			jump = true;
		}

		if(jump)
		{
			break;
		}
	};
}


void saveDate() {
	HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
}

void changeDay(int sign, LCDDisplay& display) {
	if (sign == -1 && date.Date == 1) {
		date.Date = 31;
	} else if (sign == 1 && date.Date == 31) {
		date.Date = 1;
	} else {
		date.Date = (uint8_t) ((int) date.Date + sign)%32;
	}
	sprintf(date_buffer, "%2d.%02d.%02d r.", date.Date, date.Month, date.Year);
	display.drawString(35, 75, date_buffer);
}

void changeMonth(int sign, LCDDisplay& display) {
	if (sign == -1 && date.Month == 1) {
		date.Month = 12;
	} else if (sign == 1 && date.Month == 12) {
		date.Month = 1;
	} else {
		date.Month = (uint8_t) ((int) date.Month + sign)%13;
	}
	sprintf(date_buffer, "%2d.%02d.%02d r.", date.Date, date.Month, date.Year);
	display.drawString(35, 75, date_buffer);
}

void changeYear(int sign, LCDDisplay& display) {
	if (sign == -1 && date.Year == 0) {
		date.Year = 99;
	} else {
		date.Year = (uint8_t) ((int) date.Year + sign)%100;
	}
	sprintf(date_buffer, "%2d.%02d.%02d r.", date.Date, date.Month, date.Year);
	display.drawString(35, 75, date_buffer);
}


static void draw_background(LCDDisplay& display)
{
	display.clear(background);

	//back button
	display.fillRect(0, 0, 50, 40, back_button_color);
	display.drawString(10, 10, "X");
	//save button
	display.fillRect(190, 0, 50, 40, save_button_color);
	display.drawString(210, 10, "S");
	//top bar
	display.fillRect(50, 0, 140, 40, bar_color);
	display.drawString(60, 10, "Date");

	// day
	//left button
	display.fillRect(0, 100, 118, 40, navigation_color);
	display.drawString(50, 110, "D+");
	//right button
	display.fillRect(122, 100, 118, 40, navigation_color);
	display.drawString(180, 110, "D-");
	//separator
	display.fillRect(118, 100, 4, 40, separator_color);

	// month
	//left button
	display.fillRect(0, 150, 118, 40, navigation_color);
	display.drawString(50, 160, "M+");
	//right button
	display.fillRect(122, 150, 118, 40, navigation_color);
	display.drawString(180, 160, "M-");
	//separator
	display.fillRect(118, 150, 4, 40, separator_color);

	// year
	//left button
	display.fillRect(0, 200, 118, 40, navigation_color);
	display.drawString(50, 210, "Y+");
	//right button
	display.fillRect(122, 200, 118, 40, navigation_color);
	display.drawString(180, 210, "Y-");
	//separator
	display.fillRect(118, 200, 4, 40, separator_color);

}
