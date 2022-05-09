#include "common_inc.h"
#include "audio.h"
#include <math.h>

Audio 	audio(&hi2s3, &hi2c3);
 
#define 	PI 			(3.1415926535898f)
 
#define		PCM_SAMP_NUM 	(882)
int16_t 	pcmsample_16bit[PCM_SAMP_NUM];

void generate_sin_16bit_double_channel(int16_t *pcm, 
									   uint32_t len, 
									   float fs_hz,
									   float f_hz, 
									   int16_t abs)
{
	float t_total_s = 1.0f / fs_hz * len / 2;
	for (uint32_t i = 0; i < len; i += 2)
	{		
		float t	  = (float)i / len * t_total_s;
		float res = sin(2 * PI * f_hz * t);
		int32_t value_mix_l	= res * abs;
		int32_t value_mix_r	= res * abs;
	 
		pcm[i + 0] = (int16_t)(value_mix_l);
		pcm[i + 1] = (int16_t)(value_mix_r); 		
	}
}

void TaskAudioEntry(void)
{	
	audio.Init();
	osDelay(100);

#if 1
	generate_sin_16bit_double_channel(pcmsample_16bit, 
									  PCM_SAMP_NUM, 
									  44100, 
									  500, 
									  32768 / 2); 	
	for (uint32_t i = 0; i < 300; i++)
	{
		audio.WriteData(pcmsample_16bit, PCM_SAMP_NUM, true);
	}
	
	float hz = 0;
	float cycle = 0;
	uint32_t num = 0;
	
	while (1)
	{		
		for (uint32_t i = 100; i < 1000; i+=2)
		{
			hz = i;
			cycle = 1.0f / hz;
			num = cycle * 44100.0f;
			__NOP();
			generate_sin_16bit_double_channel(pcmsample_16bit, 
											  num, 
											  44100, 
											  i, 
											  32768 / 2); 				
			audio.WriteData(pcmsample_16bit, num, true);
		}	
		for (uint32_t i = 1000; i > 100; i-=2)
		{
			hz = i;
			cycle = 1.0f / hz;
			num = cycle * 44100.0f;		
			__NOP();
			generate_sin_16bit_double_channel(pcmsample_16bit, 
											  num, 
											  44100, 
											  i, 
											  32768 / 2); 				
			audio.WriteData(pcmsample_16bit, num, true);
		}			
	}
#endif
}
 
