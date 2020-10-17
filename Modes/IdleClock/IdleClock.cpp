#include "PeripheralsPack.hpp"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdio.h>

extern LPTIM_HandleTypeDef hlptim1;
extern RTC_HandleTypeDef hrtc;

extern uint8_t user_resumed;

static void updateClock(LCDDisplay* display);

uint8_t idleClock(uint8_t* modes_stack, PeripheralsPack* pack) {
	pack->lcd_display.clear(0xAEDB);
	pack->lcd_display.setBackgroundColor(0xAEDB);
	pack->lcd_display.setTextColor(0x19AA);
	for(uint8_t i = 0; i <= 1; ++i) {
		pack->lcd_display.setBacklight(i);
		HAL_Delay(5);
	}

    user_resumed = false;

	while(!user_resumed) {
		//HAL_LPTIM_Counter_Start_IT(&hlptim1, 15360);
		updateClock(&(pack->lcd_display));

		if(HAL_LPTIM_Counter_Start_IT(&hlptim1, 15360) != HAL_OK) {
			Error_Handler();
		}
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();
	}

    user_resumed = false;

	return 0;
}

void updateClock(LCDDisplay* display) {
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	char time_buffer[6];
	char date_buffer[11];

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	sprintf(time_buffer, "%2d:%02d", time.Hours, time.Minutes);
	sprintf(date_buffer, "%2d.%02d.%04d", date.Date, date.Month, date.Year);

	display->drawString(77, 90, time_buffer);
	display->drawString(35, 120, date_buffer);
}

