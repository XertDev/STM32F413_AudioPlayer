#include "MainMenu.hpp"
#include "Utils.hpp"
#include "stm32f4xx_hal.h"


extern LPTIM_HandleTypeDef hlptim1;

extern bool timeout_lptim;
extern bool detected_touch;

static void draw_background(LCDDisplay& display);
static void draw_settings_button(LCDDisplay& display, bool highlighted=false);
static void draw_player_button(LCDDisplay& display, bool highlighted=false);


constexpr Color background = from_r8g8b8(238, 244, 237);
constexpr Color shade = from_r8g8b8(222, 235, 220);

constexpr Color player_color = from_r8g8b8(255, 0, 0);
constexpr Color settings_color = from_r8g8b8(0, 0, 255);


constexpr uint8_t target_backlight_level = 100;


void mainMenu(uint8_t* modes_stack, PeripheralsPack& pack)
{
	draw_background(pack.lcd_display);
	for(uint8_t i = pack.lcd_display.backlight(); i <= target_backlight_level; ++i)
	{
		pack.lcd_display.setBacklight(i);
		HAL_Delay(5);
	}
	draw_settings_button(pack.lcd_display);
	draw_player_button(pack.lcd_display);

	while(true)
	{
		auto& touch_panel = pack.touch_panel;
		bool jump = false;

		if(HAL_LPTIM_Counter_Start_IT(&hlptim1, 256*30) != HAL_OK) {
			Error_Handler();
		}

		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(detected_touch)
		{
			HAL_LPTIM_Counter_Stop_IT(&hlptim1);
			if(touch_panel.detectTouch() == 1)
			{
				auto touch_details = touch_panel.getDetails(0);
				if(touch_details.event_type == 1)
				{
					auto touch_info = touch_panel.getPoint(0);
					if(inRange(touch_info.x, 80, 160) && inRange(touch_info.y, 130, 210))
					{
						uint8_t* last = modes_stack;
						while(*last != 0)
						{
							++last;
						}
						*last = 3;
						jump = true;
					}
					else if(inRange(touch_info.x, 80, 160) && inRange(touch_info.y, 30, 110))
					{
						uint8_t* last = modes_stack;
						while(*last != 0)
						{
							++last;
						}
						*last = 4;
						jump = true;
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
		timeout_lptim = false;

		if(jump)
		{
			break;
		}
	}
}

static void draw_background(LCDDisplay& display)
{
	display.clear(background);

	//shade behind settings button
	display.fillRect(20, 90,10, 80, shade);
	display.fillRect(30, 160, 70, 10, shade);

	//shade behind player button
	display.fillRect(120, 90, 10, 80, shade);
	display.fillRect(130, 160, 70, 10, shade);
}

static void draw_settings_button(LCDDisplay& display, bool highlighted)
{
	display.fillRect(30, 80, 80, 80, settings_color);
	display.setBackgroundColor(settings_color);
	display.drawString(45, 110, "Set");
}

static void draw_player_button(LCDDisplay& display, bool highlighted)
{
	display.fillRect(130, 80, 80, 80, player_color);
	display.setBackgroundColor(player_color);
	display.drawString(135, 110, "List");
}

