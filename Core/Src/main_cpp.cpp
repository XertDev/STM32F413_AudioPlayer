#include "../stm32f4xx_hal.h"
#include "IdleClock/IdleClock.hpp"
#include "PeripheralsPack.hpp"
#include "MainMenu/MainMenu.hpp"
#include "Settings/Settings.hpp"
#include "SongList/SongList.hpp"
#include "SetTime/SetTime.hpp"
#include "SetDate/SetDate.hpp"
#include "Player/Player.hpp"
#include <cmath>

extern TIM_HandleTypeDef htim9;
extern SD_HandleTypeDef hsd;
extern FMPI2C_HandleTypeDef hfmpi2c1;
extern I2S_HandleTypeDef hi2s2;

extern bool detected_touch;


void resetFMPI2C() {
	HAL_FMPI2C_DeInit(&hfmpi2c1);
	  hfmpi2c1.Instance = FMPI2C1;
	  hfmpi2c1.Init.Timing = 0x00606092;
	  hfmpi2c1.Init.OwnAddress1 = 0;
	  hfmpi2c1.Init.AddressingMode = FMPI2C_ADDRESSINGMODE_7BIT;
	  hfmpi2c1.Init.DualAddressMode = FMPI2C_DUALADDRESS_DISABLE;
	  hfmpi2c1.Init.OwnAddress2 = 0;
	  hfmpi2c1.Init.OwnAddress2Masks = FMPI2C_OA2_NOMASK;
	  hfmpi2c1.Init.GeneralCallMode = FMPI2C_GENERALCALL_DISABLE;
	  hfmpi2c1.Init.NoStretchMode = FMPI2C_NOSTRETCH_DISABLE;
	  if (HAL_FMPI2C_Init(&hfmpi2c1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Configure Analogue filter
	  */
	  if (HAL_FMPI2CEx_ConfigAnalogFilter(&hfmpi2c1, FMPI2C_ANALOGFILTER_ENABLE) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

LCDIOSettings setting {
	htim9,
	TIM_CHANNEL_1,
	{LCD_TE_GPIO_Port, LCD_TE_Pin},
	{LCD_CTP_RST_GPIO_Port, LCD_CTP_RST_Pin},
	(LCDController*)(0x60000000 | 0x08000000)
};

uint8_t touches = 0;
uint8_t gesture = 0;
touch::TouchPoint point;
touch::TouchDetails details;

extern "C" void main_cpp();
void main_cpp()
{
	PeripheralsPack pack{
		LCDDisplay(setting),
		touch::TouchPanel(&hfmpi2c1, 0x70, 240, 240, &resetFMPI2C),
		Storage(),
		audio::AudioCodec(&hfmpi2c1, &hi2s2, 0x34, &resetFMPI2C)
	};

	pack.lcd_display.init();
	pack.touch_panel.setPollingMode();

	pack.lcd_display.setOrientation(ST7789H2::ORIENTATION::LANDSCAPE_ROT180);
	//idleClock(NULL, &pack);
	pack.touch_panel.id();
	pack.touch_panel.setThreshhold(20);

	auto test  =pack.codec.id();

	pack.codec.init(audio::OUTPUT_DEVICE::HEADPHONE, audio::FREQUENCY::FREQ_44K);
	pack.codec.setVolume(10);

	#define BUFFER_SIZE		2200
	pack.storage.init(hsd);
//	FIL file;
//	pack.storage.openFile("0:/test2.wav", file);
//	uint8_t sound[8172];
//	int8_t header[44];
//	unsigned int br;
//	f_read(&file, header, 44, &br);
//	pack.codec.setVolume(50);
//	while(true)
//	{
//		f_read(&file, sound, sizeof(sound), &br);
//        HAL_I2S_Transmit(&hi2s2, (uint16_t*)sound, sizeof(sound)/2, HAL_MAX_DELAY);
//	}


	uint8_t modes_stack[16] = {1, 0};
	void (*modes[])(uint8_t* modes_stack, PeripheralsPack& pack) =
	{
			mainMenu,
			idleClock,
			settings,
			songList,
			setTime,
			setDate,
			player
	};


	while(true)
	{
		uint8_t next;
		uint8_t* tmp = modes_stack;
		while(*tmp != 0)
		{
			next = *tmp;
			++tmp;
		}
		modes[next-1](modes_stack, pack);
	}

	//main_menu(modes_stack, pack);
	//song_list(modes_stack, pack);


//	static int16_t audio_data[2 * BUFFER_SIZE];
//    for (int i = 0; i < BUFFER_SIZE; i++) {
//        int16_t value = (int16_t)(32000.0 * sin(2.0 * 3.14 * i / 22.0));
//        audio_data[i * 2] = value;
//        audio_data[i * 2 + 1] = value;
//    }
//
//    for(int i = 0; i < 100; ++i)
//    {
//    	pack.codec.setVolume(i);
//    	HAL_Delay(100);
//        HAL_I2S_Transmit(&hi2s2, (uint16_t*)audio_data, 2 * BUFFER_SIZE, HAL_MAX_DELAY);
//
//    }


  bool last_state = false;
  while (1)
  {
	  while(detected_touch){
		  touches = pack.touch_panel.detectTouch();
		  if(touches > 0) {
			  point = pack.touch_panel.getPoint(0);
			  details = pack.touch_panel.getDetails(0);
			  gesture = pack.touch_panel.getGesture();
		  }
	  }
	  uint8_t test = 0;
	  if(BSP_SD_IsDetected())
	  {
		  if(!last_state)
		  {
			  pack.lcd_display.displayOn();
			  int i = 0;
			  while(i <= 100) {
				  pack.lcd_display.setBacklight(i);
				  HAL_Delay(5);
				  ++i;
			  }
			  pack.lcd_display.clear(0xAA);



			  pack.storage.init(hsd);
			  pack.storage.entriesInDirectoryCount("0:", test);
			  for(uint8_t i = 0; i < test; ++i) {
				  HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET);
				  HAL_Delay(1000);
				  HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET);
				  HAL_Delay(500);
			  }
			  pack.storage.deInit();
			  pack.lcd_display.clear(0x07E0);
			  HAL_Delay(2000);
			  pack.lcd_display.clear(0xA145);
			  pack.lcd_display.setBackgroundColor(0x07E0);
			  pack.lcd_display.drawString(77, 40, "teraz");
			  pack.lcd_display.drawRect(70, 30, 96, 44, 0x07E0);
			  pack.lcd_display.setBackgroundColor(0xA145);
			  pack.lcd_display.drawString(69, 100, "DZIALA");
			  pack.lcd_display.drawRect(60, 90, 120, 44, 0x07E0);
		  }

		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
		  last_state = true;
	  }
	  else
	  {
		  if(last_state)
		  {
			  int i = 100;
			  while(i >= 0) {
				  pack.lcd_display.setBacklight(i);
				  HAL_Delay(5);
				  --i;
			  }
			  pack.lcd_display.displayOff();
			  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

		  }
		  last_state = false;
	  }
  }
}
