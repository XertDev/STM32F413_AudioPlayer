#include "SongList.hpp"
#include <memory.h>

extern bool detected_touch;

constexpr Color background = from_r8g8b8(238, 244, 237);
constexpr Color back_button_color = from_r8g8b8(255, 0, 0);
constexpr Color navigation_color = from_r8g8b8(0, 0, 120);
constexpr Color separator_color = from_r8g8b8(0, 0, 0);


constexpr Color bar_color = from_r8g8b8(150, 150, 150);


constexpr uint8_t target_backlight_level = 100;


static void draw_background(LCDDisplay& display);
static void update_file_list(char* entries, LCDDisplay& display);



void songList(uint8_t* modes_stack, PeripheralsPack& pack)
{
		draw_background(pack.lcd_display);
		pack.lcd_display.setBackgroundColor(background);
		for(uint8_t i = pack.lcd_display.backlight(); i <= target_backlight_level; ++i)
		{
			pack.lcd_display.setBacklight(i);
			HAL_Delay(5);
		}

		uint16_t directory_offset = 0;
		bool update_list = true;
		auto scanner = pack.storage.entriesInDirectoryScanner("/");
		char entries[3 * 18];
		memset(entries, 0, 18*3);

		for(int i = 0; i < 3; ++i)
		{
			if(scanner.valid())
			{
				auto& file_info = scanner.fileInfo();
				strncpy(&(entries[i*18]), file_info.fname, 17);
			}
			scanner.next();
		}
		while(true){
			if(update_list)
			{
				update_file_list(entries, pack.lcd_display);
				update_list = false;
			}
			HAL_SuspendTick();
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
			HAL_ResumeTick();
			auto action_point = touch::TouchPoint{0, 0};
			while(detected_touch)
			{
				if(pack.touch_panel.detectTouch() == 1)
				{
					for(int i = 0; i < 3; ++i)
					{
						if(scanner.valid())
						{
							auto& file_info = scanner.fileInfo();
							memset(&(entries[i*18]), 0, 18);
							strncpy(&(entries[i*18]), file_info.fname, 17);
							update_list = true;
						}
						scanner.next();
					}
				}
			}
		};
}

static void draw_background(LCDDisplay& display)
{
	display.clear(background);

	//back button
	display.fillRect(0, 0, 50, 30, back_button_color);
	//top bar
	display.fillRect(50, 0, 190, 30, bar_color);

	//left button
	display.fillRect(0, 200, 118, 40, navigation_color);
	//right button
	display.fillRect(122, 200, 118, 40, navigation_color);
	//separator
	display.fillRect(118, 200, 4, 40, separator_color);

}

static void update_file_list(char* entries, LCDDisplay& display)
{
	display.fillRect(0, 30, 240, 170, background);
	for(int i = 0; i < 3; ++i)
	{
		display.drawString(10, 40 + 50*i, &(entries[i*18]));
	}
}
