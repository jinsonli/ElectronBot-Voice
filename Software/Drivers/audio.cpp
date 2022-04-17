#include "audio.h"

#define	WM8978_REG_MAX				(58)
#define	WM8978_ADDRESS				0x1A
#define	WM8978_WIRTE_ADDRESS		(WM8978_ADDRESS << 1 | 0)

#define USE_OS_TOOLS

#ifdef USE_OS_TOOLS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#ifdef USE_OS_TOOLS

static SemaphoreHandle_t sema_i2s_tx;

static int32_t semp_i2s_tx_init()
{
	sema_i2s_tx = xSemaphoreCreateBinary();
    if(sema_i2s_tx == NULL)
    {
		return -1;
    }
	return 0;
}

static void semp_i2s_tx_wait()
{
	if (sema_i2s_tx != NULL)
	{
		xSemaphoreTake(sema_i2s_tx, portMAX_DELAY);
	}
}

static void semp_i2s_tx_give()
{
	if (sema_i2s_tx != NULL)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(sema_i2s_tx, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

extern "C" void audio_i2s_tx_done()
{
	semp_i2s_tx_give();
}

#endif

static uint16_t Wm8978RegTbl[WM8978_REG_MAX] =
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 

uint16_t Audio::ReadReg(uint8_t _reg)
{	
	return  (_reg < WM8978_REG_MAX) ? Wm8978RegTbl[_reg] : 0;
}

void Audio::WriteReg(uint8_t _reg, uint16_t _data)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t pdata[10] =	{ 0 };
 
	pdata[0] = (_reg << 1) | ((_data >> 8) & 0x01);
	pdata[1] = (_data & 0xFF);
	
	ret = HAL_I2C_Master_Transmit(i2c,
								  WM8978_WIRTE_ADDRESS, 
								  pdata, 
								  2, 
								  HAL_MAX_DELAY);
	if ((ret == HAL_OK) 
	 && (_reg < WM8978_REG_MAX))
	{
		Wm8978RegTbl[_reg] = _data;	
	}	
}
 
/*
	0 	= -12 DB
	16 	= + 0 DB
*/
void Audio::SetMicGain(uint8_t _gain)
{
	_gain &= 0X3F;
	WriteReg(45, _gain);	 
	WriteReg(46, _gain|1<<8); 
}

/*
	
*/
void Audio::SetHeadPhoneVol(uint8_t _voll, uint8_t _volr)
{
	_voll &= 0X3F;
	_volr &= 0X3F; 
	if(_voll == 0)
	{
		_voll |= 1<<6; 
	}
	if(_volr == 0)
	{
		_volr |= 1<<6;
	}
	WriteReg(52, _voll);	 
	WriteReg(53, _volr | (1<<8));
}

void Audio::SetSpeakerVol(uint8_t _volx)
{ 
	_volx &= 0X3F;
	if(_volx == 0)
	{
		_volx |= 1<<6;
	}
 	WriteReg(54, _volx);	
	WriteReg(55, _volx | (1<<8));
}

void Audio::Init(void)
{
#ifdef USE_OS_TOOLS
	semp_i2s_tx_init();
#endif
	
	WriteReg(0,  0X000); // reset
	WriteReg(1,  0X01B);
	WriteReg(2,  0X1BF); 
	WriteReg(3,  0X06F);
	WriteReg(6,  0X000);	
	WriteReg(43, 0X010);
	WriteReg(47, 0X100);
	WriteReg(48, 0X100);
	WriteReg(49, 0X002);
	WriteReg(10, 0X008);
	WriteReg(14, 0X008);
	
	SetMicGain(57);  //57
	SetSpeakerVol(60); 

	//WriteReg(50, 1 << 1 | 5 << 2 | 0X01);  // by pass L
	//WriteReg(51, 1 << 1 | 5 << 2 | 0X01);  // by pass R		
}

/*
	vol = [0 - 100]
*/
void Audio::SetVol(uint8_t _vol)
{
	SetSpeakerVol(_vol);
}

void Audio::SetBusy(bool _busy)
{
	_isBusy = _busy;
}

bool Audio::IsBusy(void)
{
	return _isBusy;
}

void Audio::WriteData(int16_t *_data, uint32_t _len, bool _useDma)
{
	if (_useDma)
	{
		_isBusy = true;
		HAL_I2S_Transmit_DMA(i2s, (uint16_t*)_data, (uint16_t)(_len));		
#ifdef USE_OS_TOOLS
		semp_i2s_tx_wait();
#else
		while (_isBusy == true);
#endif			
	}
	else
	{
		_isBusy = true;
		HAL_I2S_Transmit(i2s, (uint16_t*)_data, (uint16_t)(_len), HAL_MAX_DELAY);
		_isBusy = false;
	}
}

void Audio::WriteRead(uint16_t *_tx, uint16_t *_rx, uint16_t _len)
{
	HAL_I2SEx_TransmitReceive_DMA(i2s, _tx, _rx, _len);	
#ifdef USE_OS_TOOLS
		semp_i2s_tx_wait();
#else
		while (_isBusy == true);
#endif		
}

void Audio::RecordCtrl(uint32_t start_stop)
{
 	
}


