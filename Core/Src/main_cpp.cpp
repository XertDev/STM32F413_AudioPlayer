#include "Storage/Storage.hpp"
#include "LCD/LCDDisplay.hpp"
#include "LCD/LCDIOSettings.hpp"
#include "LCD/LCDConstants.hpp"
#include "Touch/TouchPanel.hpp"
#include "../stm32f4xx_hal.h"
#include "fatfs.h"

extern TIM_HandleTypeDef htim9;
extern SD_HandleTypeDef hsd;
extern FMPI2C_HandleTypeDef hfmpi2c1;

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



extern "C" void main_cpp();
void main_cpp()
{
	LCDDisplay display(setting);


	auto d_error = display.init();
	TouchPanel t_panel(&hfmpi2c1, 0x70, &resetFMPI2C);
	auto test = t_panel.id();
	t_panel.setPollingMode();

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
