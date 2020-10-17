#ifndef INC_PERIPHERALSPACK_HPP_
#define INC_PERIPHERALSPACK_HPP_

#include "LCD/LCDDisplay.hpp"
#include "Touch/TouchPanel.hpp"
#include "Storage/Storage.hpp"



struct PeripheralsPack
{
	LCDDisplay lcd_display;
	TouchPanel touch_panel;
	Storage storage;
};


#endif /* INC_PERIPHERALSPACK_HPP_ */
