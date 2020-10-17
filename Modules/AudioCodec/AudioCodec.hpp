#ifndef AUDIOCODEC_AUDIOCODEC_HPP_
#define AUDIOCODEC_AUDIOCODEC_HPP_

#include "stm32f4xx_hal.h"

enum class AudioError {
	OK=0
};

class AudioCodec {
public:
	AudioCodec(FMPI2C_HandleTypeDef hfmpi2c1, I2S_HandleTypeDef hi2s2);
	~AudioCodec();

	uint32_t id();

	void init();

private:
	FMPI2C_HandleTypeDef hfmpi2c1;
	I2S_HandleTypeDef hi2s2;
};

#endif /* AUDIOCODEC_AUDIOCODEC_HPP_ */
