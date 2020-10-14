/*
 * LCDDisplay.hpp
 *
 *  Created on: Oct 14, 2020
 *      Author: xert
 */

#ifndef LCD_LCDDISPLAY_HPP_
#define LCD_LCDDISPLAY_HPP_

#include "stm32f4xx_hal.h"

class LCDDisplay {
public:
	LCDDisplay(TIM_HandleTypeDef& backlight_pwm, uint32_t backlight_channel);
	~LCDDisplay();

	void setBacklight(uint8_t level);
private:
	TIM_HandleTypeDef& backlight_pwm_;
	uint32_t backlight_channel_;

	uint8_t level_;
};

#endif /* LCD_LCDDISPLAY_HPP_ */
