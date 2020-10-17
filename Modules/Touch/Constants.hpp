#ifndef TOUCH_CONSTANTS_HPP_
#define TOUCH_CONSTANTS_HPP_

#include "stdint.h"


namespace FT6206 {
	enum class ID {
		UNKNOWN	 	= 0x00,
		FT6206		= 0x11,
		FT6x36		= 0xCD
	};

	enum class REG: uint16_t {
		ID 		= 0xA8,
		GMODE 	= 0xA4
	};

	namespace FLAG {
		constexpr uint8_t G_MODE_INTERRUPT_TRIGGER = 0x01;
		constexpr uint8_t G_MODE_INTERRUPT_POLLING = 0x00;
		constexpr uint8_t G_MODE_INTERRUPT_MASK    = 0x03;
	};
}


#endif /* TOUCH_CONSTANTS_HPP_ */
