#include "SongList.hpp"
#include <memory.h>
#include "Utils.hpp"

extern bool detected_touch;
extern bool timeout_lptim;

extern LPTIM_HandleTypeDef hlptim1;

extern char** filenames;
extern int files_size;
extern int file_index;

constexpr Color background = from_r8g8b8(238, 244, 237);
constexpr Color back_button_color = from_r8g8b8(255, 0, 0);
constexpr Color navigation_color = from_r8g8b8(0, 0, 120);
constexpr Color separator_color = from_r8g8b8(0, 0, 0);


constexpr Color bar_color = from_r8g8b8(150, 150, 150);


constexpr uint8_t target_backlight_level = 100;


static void draw_background(LCDDisplay& display);
static void update_file_list(char** entries, int index, LCDDisplay& display);

void songList(uint8_t* modes_stack, PeripheralsPack& pack)
{
		draw_background(pack.lcd_display);
		pack.lcd_display.setBackgroundColor(background);
		for(uint8_t i = pack.lcd_display.backlight(); i <= target_backlight_level; ++i)
		{
			pack.lcd_display.setBacklight(i);
			HAL_Delay(5);
		}

		int index = 0;
		update_file_list(filenames, index, pack.lcd_display);

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
							*last = 0;
							jump = true;
						} else if(inRange(touch_info.x, 50, 100) && inRange(touch_info.y, 0, 240))
						{
							file_index = index;
							uint8_t* last = modes_stack;
							while(*last != 0)
							{
								++last;
							}
							--last;
							*last = 7;
							jump = true;
						} else if(inRange(touch_info.x, 100, 150) && inRange(touch_info.y, 0, 240))
						{
							if(index+1 < files_size) {
								file_index = index+1;
								uint8_t* last = modes_stack;
								while(*last != 0)
								{
									++last;
								}
								--last;
								*last = 7;
								jump = true;
							}
						} else if(inRange(touch_info.x, 150, 200) && inRange(touch_info.y, 0, 240))
						{
							if(index+2 < files_size) {
								file_index = index+2;
								uint8_t* last = modes_stack;
								while(*last != 0)
								{
									++last;
								}
								--last;
								*last = 7;
								jump = true;
							}
						} else if(inRange(touch_info.x, 200, 240) && inRange(touch_info.y, 120, 240))
						{
							if(index-3 >= 0) {
								index -= 3;
								update_file_list(filenames, index, pack.lcd_display);
							}
						} else if(inRange(touch_info.x, 200, 240) && inRange(touch_info.y, 0, 120))
						{
							if(index+3 < files_size) {
								index += 3;
								update_file_list(filenames, index, pack.lcd_display);
							}
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
	display.drawString(60, 10, "List");

	//left button
	display.fillRect(0, 200, 118, 40, navigation_color);
	display.setBackgroundColor(navigation_color);
	display.drawString(20, 210, "PREV");
	//right button
	display.fillRect(122, 200, 118, 40, navigation_color);
	display.drawString(140, 210, "NEXT");
	//separator
	display.fillRect(118, 200, 4, 40, separator_color);

}

static void update_file_list(char** entries, int index, LCDDisplay& display)
{
	display.fillRect(0, 40, 240, 160, background);
	for(int i = 0; i < 3; ++i)
	{
		if(index+i < files_size) {
			char name[18];
			name[17] = 0;
			strncpy(name, entries[index+i], 17);
			display.drawString(10, 65 + 50*i, entries[index+i]);
		}
	}
	for(int i = 0; i < 2; ++i)
	{
		display.fillRect(0, 96 + 50*i, 240, 3, bar_color);
	}
}
