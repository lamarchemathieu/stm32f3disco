
#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"
#include "serial.h"

#include "timer.h"

void timer_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

	//PCLK2 = 64000000;
	LL_RCC_SetTIMClockSource(LL_RCC_TIM1_CLKSOURCE_PCLK2);

	uint32_t tim_freq = LL_RCC_GetTIMClockFreq(LL_RCC_TIM1_CLKSOURCE);

	LL_TIM_InitTypeDef tim;
	LL_TIM_StructInit(&tim);
	tim.Prescaler = 64000 - 1;//64M / 64k = 1k => 1ms
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Autoreload = 1000 - 1;//Count from 0 to 999 => 1k/1000 = 1s
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.RepetitionCounter = 0;

	LL_TIM_Init(TIM1, &tim);
	LL_TIM_EnableCounter(TIM1);

	LL_TIM_EnableIT_UPDATE(TIM1);
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

}

uint16_t timer_get(void)
{
	return LL_TIM_GetCounter(TIM1);
}

void TIM1_UP_TIM16_IRQHandler(void)
{
	serial_print("TIM1_UP_TIM16_IRQHandler\r\n");
	LL_TIM_ClearFlag_UPDATE(TIM1);
}
