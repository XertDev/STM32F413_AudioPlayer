#include "Storage/Storage.hpp"
#include "LCD/LCDDisplay.hpp"
#include "LCD/LCDIOSettings.hpp"
#include "LCD/LCDConstants.hpp"
#include "../stm32f4xx_hal.h"
#include "fatfs.h"

extern TIM_HandleTypeDef htim9;
extern SD_HandleTypeDef hsd;

LCDIOSettings setting {
	htim9,
	TIM_CHANNEL_1,
	{LCD_TE_GPIO_Port, LCD_TE_Pin},
	{LCD_CTP_RST_GPIO_Port, LCD_CTP_RST_Pin},
	(LCDController*)(0x60000000 | 0x08000000)
};


extern "C" void main_cpp();
void main_cpp()
{
	LCDDisplay display(setting);

	auto d_error = display.init();
	display.setOrientation(ST7789H2::ORIENTATION::PORTRAIT);
	  HAL_Delay(50);

  bool last_state = false;
  while (1)
  {
	  uint8_t test = 0;
	  if(BSP_SD_IsDetected())
	  {
		  if(!last_state)
		  {
			  display.displayOn();
			  int i = 0;
			  while(i <= 100) {
				  display.setBacklight(i);
				  HAL_Delay(5);
				  ++i;
			  }
				display.clear(0xAA);

			  Storage storage;

			  StorageErrors error = storage.init(hsd);
			  error = storage.entriesInDirectoryCount("0:", test);
			  for(uint8_t i = 0; i < test; ++i) {
				  HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET);
				  HAL_Delay(1000);
				  HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET);
				  HAL_Delay(500);
			  }
			  storage.deInit();
			  display.clear(0x07E0);
			  HAL_Delay(2000);
			  display.clear(0xA145);
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
				  display.setBacklight(i);
				  HAL_Delay(5);
				  --i;
			  }
			  display.displayOff();
			  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

		  }
		  last_state = false;
	  }
	  HAL_Delay(1000);
  }
}
