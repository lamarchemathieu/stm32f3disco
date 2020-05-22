
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_bus.h"

uint32_t tab[] = {
	LL_GPIO_PIN_8,
	LL_GPIO_PIN_9,
	LL_GPIO_PIN_10,
	LL_GPIO_PIN_11,
	LL_GPIO_PIN_12,
	LL_GPIO_PIN_13,
	LL_GPIO_PIN_14,
	LL_GPIO_PIN_15,
};

void delay()
{
	uint32_t i;

	for(i=0;i<100000;i++)
	{
		asm ( "nop" );
	}
}

int main(void)
{
	uint32_t cpt = 0;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);

	LL_GPIO_InitTypeDef gpios;
	LL_GPIO_StructInit(&gpios);

	gpios.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	gpios.Mode = LL_GPIO_MODE_OUTPUT;
	gpios.Pull = LL_GPIO_PULL_NO;
	gpios.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpios.Speed      = LL_GPIO_SPEED_FREQ_HIGH;

	LL_GPIO_Init(GPIOE, &gpios);

	while (1)
	{
		LL_GPIO_SetOutputPin(GPIOE, tab[cpt]);
		delay();
		LL_GPIO_ResetOutputPin(GPIOE, tab[cpt]);

		cpt++;

		if (cpt >= sizeof(tab)/sizeof(uint32_t))
			cpt = 0;
	}
}
