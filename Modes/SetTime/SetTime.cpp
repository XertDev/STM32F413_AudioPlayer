#include "SetTime.hpp"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include "Utils.hpp"

extern RTC_HandleTypeDef hrtc;
extern LPTIM_HandleTypeDef hlptim1;

RTC_TimeTypeDef time;
char time_buffer[6];

constexpr Color background = from_r8g8b8(238, 244, 237);
constexpr Color back_button_color = from_r8g8b8(255, 0, 0);
constexpr Color save_button_color = from_r8g8b8(0, 255, 0);
constexpr Color navigation_color = from_r8g8b8(0, 0, 120);
constexpr Color separator_color = from_r8g8b8(0, 0, 0);
constexpr Color bar_color = from_r8g8b8(150, 150, 150);
constexpr uint8_t preferred_backlight = 10;

extern bool detected_touch;
extern bool timeout_lptim;

static void saveTime();
static void changeHours(int sign, LCDDisplay& display);
static void changeMinutes(int sign, LCDDisplay& display);
static void draw_background(LCDDisplay& display);

void setTime(uint8_t* modes_stack, PeripheralsPack& pack) {
	draw_background(pack.lcd_display);
	pack.lcd_display.setBackgroundColor(background);

	pack.lcd_display.setTextColor(0x19AA);
	for(uint8_t i = pack.lcd_display.backlight(); i <= preferred_backlight; ++i) {
		pack.lcd_display.setBacklight(i);
		HAL_Delay(5);
	}

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	sprintf(time_buffer, "%2d:%02d", time.Hours, time.Minutes);
	pack.lcd_display.drawString(77, 90, time_buffer);

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
						saveTime();
						uint8_t* last = modes_stack;
						while(*last != 0)
						{
							++last;
						}
						--last;
						*last = 3;
						jump = true;
					} else if(inRange(touch_info.x, 150, 190) && inRange(touch_info.y, 120, 240))
					{
						changeHours(1, pack.lcd_display);
					} else if(inRange(touch_info.x, 150, 190) && inRange(touch_info.y, 0, 120))
					{
						changeHours(-1, pack.lcd_display);
					} else if(inRange(touch_info.x, 200, 240) && inRange(touch_info.y, 120, 240))
					{
						changeMinutes(1, pack.lcd_display);
					} else if(inRange(touch_info.x, 200, 240) && inRange(touch_info.y, 0, 120))
					{
						changeMinutes(-1, pack.lcd_display);
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


void saveTime() {
	HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
}

void changeHours(int sign, LCDDisplay& display) {
	time.Hours = (uint8_t) ((int) time.Hours + sign)%24;
	sprintf(time_buffer, "%2d:%02d", time.Hours, time.Minutes);
	display.drawString(77, 90, time_buffer);
}

void changeMinutes(int sign, LCDDisplay& display) {
	time.Minutes = (uint8_t) ((int) time.Minutes + sign)%60;
	sprintf(time_buffer, "%2d:%02d", time.Hours, time.Minutes);
	display.drawString(77, 90, time_buffer);
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

	//left button
	display.fillRect(0, 150, 118, 40, navigation_color);
	display.drawString(50, 160, "H+");
	//right button
	display.fillRect(122, 150, 118, 40, navigation_color);
	display.drawString(180, 160, "H-");
	//separator
	display.fillRect(118, 150, 4, 40, separator_color);

	//left button
	display.fillRect(0, 200, 118, 40, navigation_color);
	display.drawString(50, 210, "M+");
	//right button
	display.fillRect(122, 200, 118, 40, navigation_color);
	display.drawString(180, 210, "M-");
	//separator
	display.fillRect(118, 200, 4, 40, separator_color);

}
