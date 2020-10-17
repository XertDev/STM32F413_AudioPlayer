#ifndef LCD_Common_HPP_
#define LCD_Common_HPP_

#include <stdint.h>

struct LCDController{
	volatile uint16_t reg;
	volatile uint16_t ram;
};

typedef uint32_t Color;

struct Font {
	const uint8_t *table;
	uint16_t width;
	uint16_t height;
};

struct LCDDRawProperties {
	Color text_color;
	Color background_color;
	Font* font;
};

struct Point {
	int16_t x;
	int16_t y;
};
#endif /* LCD_COMMON_HPP_ */

