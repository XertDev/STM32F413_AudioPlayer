#include "Player.hpp"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include "Utils.hpp"
#include <memory.h>

extern RTC_HandleTypeDef hrtc;
extern LPTIM_HandleTypeDef hlptim1;
extern I2S_HandleTypeDef hi2s2;

extern FIL file;
extern uint8_t sound[8172];
extern unsigned int br;

extern char** filenames;
extern int files_size;
extern int file_index;

constexpr Color background = from_r8g8b8(238, 244, 237);
constexpr Color back_button_color = from_r8g8b8(255, 0, 0);
constexpr Color navigation_color = from_r8g8b8(0, 0, 120);
constexpr Color separator_color = from_r8g8b8(0, 0, 0);
constexpr Color bar_color = from_r8g8b8(150, 150, 150);
constexpr uint8_t preferred_backlight = 10;

extern bool detected_touch;
extern bool timeout_lptim;

static void draw_background(LCDDisplay& display);
static void draw_play_pause(LCDDisplay& display, bool paused);

void player(uint8_t* modes_stack, PeripheralsPack& pack) {
	draw_background(pack.lcd_display);
	pack.lcd_display.setBackgroundColor(background);

	pack.lcd_display.setTextColor(0x19AA);
	for(uint8_t i = pack.lcd_display.backlight(); i <= preferred_backlight; ++i) {
		pack.lcd_display.setBacklight(i);
		HAL_Delay(5);
	}

	bool jump = false;

	bool paused = false;

	char* pocz = new char[3];
	strcpy(pocz, "0:/");

	pack.storage.openFile(strcat(pocz, filenames[file_index]), file);
	int8_t header[44];
	f_read(&file, header, 44, &br);
	pack.codec.setVolume(50);
	f_read(&file, sound, sizeof(sound), &br);
	HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)sound, sizeof(sound)/2);

	while(true)
	{
//		if(HAL_LPTIM_Counter_Start_IT(&hlptim1, 15360) != HAL_OK) {
//			Error_Handler();
//		}
//		HAL_SuspendTick();
//		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//		HAL_ResumeTick();

		if(f_eof(&file))
		{
			file_index++;
			file_index %= files_size;
			HAL_I2S_DMAStop(&hi2s2);
			uint8_t* last = modes_stack;
			while(*last != 0)
			{
				++last;
			}
			--last;
			*last = 7;
			jump = true;
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
						HAL_I2S_DMAStop(&hi2s2);

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
						if (paused) {
							HAL_I2S_DMAPause(&hi2s2);
						} else {
							HAL_I2S_DMAResume(&hi2s2);
						}
						draw_play_pause(pack.lcd_display, paused);
					} else if(inRange(touch_info.x, 160, 240) && inRange(touch_info.y, 0, 120))
					{
						file_index++;
						file_index %= files_size;
						HAL_I2S_DMAStop(&hi2s2);
						uint8_t* last = modes_stack;
						while(*last != 0)
						{
							++last;
						}
						--last;
						*last = 7;
						jump = true;
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


static void draw_background(LCDDisplay& display)
{
	display.clear(background);

	//back button
	display.fillRect(0, 0, 50, 40, back_button_color);
	display.setBackgroundColor(back_button_color);
	display.drawString(10, 10, "X");
	//top bar
	display.fillRect(50, 0, 190, 40, bar_color);
	display.setBackgroundColor(bar_color);
	display.drawString(60, 10, "Player");

	char* name = new char[18];
	strncpy(name, filenames[file_index], 18);
	display.drawString(10, 90, name);
	delete name;

	//left button
	display.fillRect(0, 160, 118, 80, navigation_color);
	display.setBackgroundColor(navigation_color);
	display.drawString(10, 190, "PAUSE");
	//right button
	display.fillRect(122, 160, 118, 80, navigation_color);
	display.drawString(130, 190, "NEXT");
	//separator
	display.fillRect(118, 160, 4, 80, separator_color);

}

static void draw_play_pause(LCDDisplay& display, bool paused)
{
	display.fillRect(0, 160, 118, 80, navigation_color);
	display.setBackgroundColor(navigation_color);
	display.drawString(10, 190, paused ? "PLAY" : "PAUSE");
}
