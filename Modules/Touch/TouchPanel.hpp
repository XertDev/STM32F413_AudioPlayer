#ifndef TOUCH_TOUCHPANEL_HPP_
#define TOUCH_TOUCHPANEL_HPP_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "Constants.hpp"



class TouchPanel {
public:
	TouchPanel(FMPI2C_HandleTypeDef *fmpi2c_handler, uint8_t address, void(*reset_func)());
	~TouchPanel();

	uint16_t id();

	void setTriggerMode();
	void setPollingMode();


private:

	FMPI2C_HandleTypeDef *fmpi2c_handler_;
	uint8_t address_;

	void(*reset_func_)();



	void writeReg(FT6206::REG reg, uint8_t* buff, uint16_t count);
	uint8_t readReg(FT6206::REG reg);
	uint16_t readRegMultiple(FT6206::REG reg, uint8_t *buff, uint16_t count);

	void resetFMPI2C();
};

#endif /* TOUCH_TOUCHPANEL_HPP_ */
