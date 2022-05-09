#ifndef ELECTRONBOT_FW_TIMER_H
#define ELECTRONBOT_FW_TIMER_H

#include "stm32f4xx.h"
#include "main.h"
#include "tim.h"

class Timer
{
public:
    explicit Timer(TIM_HandleTypeDef* _timer) :
        timer(_timer)
    {				
		TIM5->CNT = 0;
	}

	uint64_t GetMicros()
	{	
		return (TIM5->CNT);
	}
	uint32_t GetMillis()
	{	
		return GetMicros() / 1000;
	}
	uint32_t GetSecond()
	{	
		return GetMicros() / 1000000.0f;
	}
private:
 
    TIM_HandleTypeDef *timer;
 
};

#endif
