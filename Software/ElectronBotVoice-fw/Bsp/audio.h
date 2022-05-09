#ifndef ELECTRONBOT_FW_AUDIO_H
#define ELECTRONBOT_FW_AUDIO_H

#include "stm32f4xx.h"
#include "main.h"
#include "i2c.h"
#include "i2s.h"

class Audio
{
public:
    explicit Audio(I2S_HandleTypeDef* _i2s, 
				   I2C_HandleTypeDef* _i2c) :
        i2s(_i2s),
		i2c(_i2c)
    {
		_isBusy = false;
	}

    void Init(void);	
	void SetVol(uint8_t vol);
	void WriteData(int16_t *_data, uint32_t _len, bool _useDma = false);
	void SetBusy(bool _busy);
	bool IsBusy(void);
	void WriteRead(uint16_t *_tx, uint16_t *_rx, uint16_t _len);
	void RecordCtrl(uint32_t start_stop);
	
private:
	
	void WriteReg(uint8_t reg, uint16_t data);
	uint16_t ReadReg(uint8_t reg);
	void SetMicGain(uint8_t gain);
	void SetHeadPhoneVol(uint8_t voll, uint8_t volr);
	void SetSpeakerVol(uint8_t volx);

	I2S_HandleTypeDef* i2s;
	I2C_HandleTypeDef* i2c;
	volatile bool _isBusy;
	
};

#endif
