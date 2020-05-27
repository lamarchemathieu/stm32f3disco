#include <stdint.h>
#include "stm32f3xx.h"
#include "stm32f3xx_ll_rcc.h"


static volatile uint32_t _critical_counter = 0;
static volatile uint64_t _tick_counter = 0;

/**
  * @brief Disable interrupts, use a counter if requested several times 
  * @param None  
  * @retval None
  */
void critical_enter(void)
{
	__disable_irq();
	++_critical_counter;
}

/**
  * @brief Eneble interrupts, if all critucal_enter have been removed
  * @param None
  * @retval None
  */
void critical_exit(void)
{
	--_critical_counter;
	if (_critical_counter == 0)
	{
		__enable_irq();
	}
}



/**
  * @brief Set the timer, stat it, and configure it to tick at 1ms.
  * @param None  
  * @retval None
  */
void tick_init(void){
	_tick_counter = 0;
	
	//Systick conficuration : 
	// CPU at 64Mhz => tick every 10ms 
	LL_RCC_ClocksTypeDef clocks;			//Declare structure named clocks to get all clocks from system
	LL_RCC_GetSystemClocksFreq(&clocks);	//Getting the clocks informations

	SysTick_Config(clocks.HCLK_Frequency / 1000);	//setting a tick on every 1ms (clock based on HCLK clock form system)
}

/**
  * @brief Get the timer counter
  * @param None  
  * @retval Timer on 64 bits
  */
uint64_t tick_get(void)
{
	uint64_t ticks;
	
	critical_enter();
	ticks = _tick_counter;
	critical_exit();
	
	return ticks;
}

/**
  * @brief Count every 1ms
  * @param None  
  * @retval None
  */ 
void SysTick_Handler(void)
{	
	critical_enter();
	//encrease counter every 1ms
	++_tick_counter;

	critical_exit();
}