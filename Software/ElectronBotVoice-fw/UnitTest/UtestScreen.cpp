#include <cmath>
#include "math.h"
#include "common_inc.h"
#include "screen.h"
#include "UnitTest.h"

Screen* lcd;

#define buf_len (30 * 240 * 3)
uint8_t lcdbuf[buf_len];	
 
extern "C"
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    lcd->isBusy = false;
}
 
void UTestScreen(void)
{
	lcd = new Screen(&hspi1);

	lcd->Init(Screen::DEGREE_0);
    lcd->SetWindow(0, 239, 0, 239);	
	
	memset(lcdbuf, 0, buf_len);	
	for (uint32_t i = 0; i < buf_len; i += 3)
	{
		lcdbuf[i] = 200;
	}
	lcd->WriteFrameBuffer(lcdbuf, buf_len);
	while (lcd->isBusy == true);	
	HAL_Delay(500);
	lcd->SetBackLight(0.0f);
}




