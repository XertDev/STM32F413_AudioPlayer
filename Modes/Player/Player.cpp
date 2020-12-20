#include "Player.hpp"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include "Utils.hpp"

extern RTC_HandleTypeDef hrtc;
extern LPTIM_HandleTypeDef hlptim1;
extern I2S_HandleTypeDef hi2s2;

constexpr Color background = from_r8g8b8(238, 244, 237);
constexpr Color back_button_color = from_r8g8b8(255, 0, 0);
constexpr Color navigation_color = from_r8g8b8(0, 0, 120);
constexpr Color separator_color = from_r8g8b8(0, 0, 0);
constexpr Color bar_color = from_r8g8b8(150, 150, 150);
constexpr uint8_t preferred_backlight = 10;

extern bool detected_touch;
extern bool timeout_lptim;

static void draw_background(LCDDisplay& display, bool paused);

void player(uint8_t* modes_stack, PeripheralsPack& pack) {
	draw_background(pack.lcd_display, true);
	pack.lcd_display.setBackgroundColor(background);

	pack.lcd_display.setTextColor(0x19AA);
	for(uint8_t i = pack.lcd_display.backlight(); i <= preferred_backlight; ++i) {
		pack.lcd_display.setBacklight(i);
		HAL_Delay(5);
	}

	bool jump = false;

	bool paused = true;

	FIL file;
	pack.storage.openFile("0:/test2.wav", file);
	uint8_t sound[8172];
	int8_t header[44];
	unsigned int br;
	f_read(&file, header, 44, &br);
	pack.codec.setVolume(50);

	while(true)
	{
//		if(HAL_LPTIM_Counter_Start_IT(&hlptim1, 15360) != HAL_OK) {
//			Error_Handler();
//		}
//		HAL_SuspendTick();
//		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//		HAL_ResumeTick();

		if (!paused) {
			f_read(&file, sound, sizeof(sound), &br);
			HAL_I2S_Transmit(&hi2s2, (uint16_t*)sound, sizeof(sound)/2, HAL_MAX_DELAY);
		}

		while(detected_touch)
		{
//			HAL_LPTIM_Counter_Stop_IT(&hlptim1);
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
						*last = 4;
						jump = true;
					} else if(inRange(touch_info.x, 160, 240) && inRange(touch_info.y, 120, 240))
					{
						paused = !paused;
						draw_background(pack.lcd_display, paused);
					} else if(inRange(touch_info.x, 160, 240) && inRange(touch_info.y, 0, 120))
					{
						paused = true;
						f_read(&file, header, 44, &br);
						draw_background(pack.lcd_display, true);
					}
				}
			}
		}

		if(jump)
		{
			break;
		}
	};
}


static void draw_background(LCDDisplay& display, bool paused)
{
	display.clear(background);

	//back button
	display.fillRect(0, 0, 50, 40, back_button_color);
	display.drawString(10, 10, "X");
	//top bar
	display.fillRect(50, 0, 190, 40, bar_color);
	display.drawString(60, 10, "Player");

	display.drawString(10, 90, "tytul piosenki");

	//left button
	display.fillRect(0, 160, 118, 80, navigation_color);
	display.drawString(10, 190, paused ? "PLAY" : "PAUSE");
	//right button
	display.fillRect(122, 160, 118, 80, navigation_color);
	display.drawString(130, 190, "STOP");
	//separator
	display.fillRect(118, 160, 4, 80, separator_color);

}
