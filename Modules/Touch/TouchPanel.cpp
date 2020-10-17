#include <Touch/TouchPanel.hpp>
#include "stm32f4xx_hal_fmpi2c.h"

constexpr auto I2C_MEMADD_SIZE_8BIT = 0x00000001U;

TouchPanel::TouchPanel(FMPI2C_HandleTypeDef *fmpi2c_handler, uint8_t address, void(*reset_func)())
:fmpi2c_handler_(fmpi2c_handler), address_(address), reset_func_(reset_func)
{
}

TouchPanel::~TouchPanel() {

}

void TouchPanel::writeReg(FT6206::REG reg, uint8_t *buff, uint16_t count) {
	auto status = HAL_OK;

	status = HAL_FMPI2C_Mem_Write(fmpi2c_handler_, address_, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, buff, count, 1000);

	if(status != HAL_OK) {
		resetFMPI2C();
	}
}

uint16_t TouchPanel::id() {
	return readReg(FT6206::REG::ID);
}

uint8_t TouchPanel::readReg(FT6206::REG reg) {
	uint8_t value[1] = {0};

	readRegMultiple(reg, value, 1);

	return value[0];
}

uint16_t TouchPanel::readRegMultiple(FT6206::REG reg, uint8_t *buff,
uint16_t count) {
	auto status = HAL_OK;
	status = HAL_FMPI2C_Mem_Read(fmpi2c_handler_, address_, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, buff, count, 1000);

	if(status) {
		resetFMPI2C();
	}

	return status;
}

void TouchPanel::setTriggerMode() {
	using namespace FT6206::FLAG;
	uint8_t value = G_MODE_INTERRUPT_TRIGGER;

	writeReg(FT6206::REG::GMODE, &value, 1);
}

void TouchPanel::setPollingMode() {
	using namespace FT6206::FLAG;
	uint8_t value = G_MODE_INTERRUPT_POLLING;

	writeReg(FT6206::REG::GMODE, &value, 1);
}

void TouchPanel::resetFMPI2C() {
	reset_func_();
}
