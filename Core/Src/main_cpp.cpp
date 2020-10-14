#include "Storage/Storage.hpp"
#include "LCD/LCDDisplay.hpp"
#include "../stm32f4xx_hal.h"
#include "fatfs.h"

extern TIM_HandleTypeDef htim9;
extern SD_HandleTypeDef hsd;

LCDDisplay display(htim9, TIM_CHANNEL_1);

extern "C" void main_cpp();
void main_cpp()
{
  bool last_state = false;
  while (1)
  {
	  uint8_t test = 0;
	  if(BSP_SD_IsDetected())
	  {
		  if(!last_state)
		  {
			  int i = 0;
			  while(i <= 100) {
				  display.setBacklight(i);
				  HAL_Delay(5);
				  ++i;
			  }
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
		  }
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
		  last_state = false;
	  }
	  HAL_Delay(1000);
  }
}
