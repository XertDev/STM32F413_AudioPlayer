/*
 * LCDDisplay.cpp
 *
 *  Created on: Oct 14, 2020
 *      Author: xert
 */

#include <LCD/LCDDisplay.hpp>

LCDDisplay::LCDDisplay(TIM_HandleTypeDef& backlight_pwm, uint32_t backlight_channel)
:backlight_pwm_(backlight_pwm), backlight_channel_(backlight_channel) {

}

LCDDisplay::~LCDDisplay() {
	HAL_TIM_PWM_Stop(&backlight_pwm_, backlight_channel_);
}

void LCDDisplay::setBacklight(uint8_t level) {
	assert_param(level <= 100);

	const auto& period = backlight_pwm_.Init.Period;

	TIM_OC_InitTypeDef config;

	config.OCMode = TIM_OCMODE_PWM1;
	config.Pulse = level/100.0f * period;
	config.OCPolarity = TIM_OCPOLARITY_HIGH;
	config.OCFastMode = TIM_OCFAST_DISABLE;

	HAL_TIM_PWM_ConfigChannel(&backlight_pwm_, &config, backlight_channel_);
	HAL_TIM_PWM_Start(&backlight_pwm_, backlight_channel_);

	level_ = level;
}
