/*
 * PinPort.hpp
 *
 *  Created on: Oct 14, 2020
 *      Author: xert
 */

#ifndef PINPORT_HPP_
#define PINPORT_HPP_

#include "stm32f4xx_hal.h"

	struct PinPort {
		GPIO_TypeDef* GPIOx;
		uint16_t GPIO_Pin;
	};


#endif /* PINPORT_HPP_ */
