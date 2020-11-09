#ifndef INC_PERIPHERALSPACK_HPP_
#define INC_PERIPHERALSPACK_HPP_

#include "LCD/LCDDisplay.hpp"
#include "Touch/touch.hpp"
#include "Storage/Storage.hpp"
#include "AudioCodec/AudioCodec.hpp"



struct PeripheralsPack
{
	LCDDisplay lcd_display;
	touch::TouchPanel touch_panel;
	Storage storage;
	audio::AudioCodec codec;
};


#endif /* INC_PERIPHERALSPACK_HPP_ */
