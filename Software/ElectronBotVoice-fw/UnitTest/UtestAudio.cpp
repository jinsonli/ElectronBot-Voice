#include "math.h"
#include "common_inc.h"
#include "audio.h"
#include "UnitTest.h"

 
Audio audio(&hi2s3, &hi2c3);

#define 	PI 			(3.1415926535898f)
#define		PCM_P_NUM 	(882)
int16_t 	pcm_16bit[PCM_P_NUM];

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

/* Unit Test Entry -------------------------------------------------------*/
void UTestAudio(void)
{
	audio.Init();		
#if 1	
	generate_sin_16bit_double_channel(pcm_16bit, 
									  PCM_P_NUM, 
									  44100, 
									  500, 
									  32768 / 2); 	
#endif	
 
	//while (1)
	for (uint32_t i = 0; i < 50; i++)
	{
#if 0	// use block transmit
		audio.WriteData(pcm_16bit, PCM_P_NUM);
#else	// use dma tranmist		
		audio.WriteData(pcm_16bit, PCM_P_NUM, true);
		//while (audio.IsBusy() == true);
#endif		
	}
}




