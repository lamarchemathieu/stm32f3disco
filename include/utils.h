#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

/**
  * @brief Disable interrupts, use a counter if requested several times 
  * @param None 
  * @retval None
  */
void critical_enter(void);

/**
  * @brief Eneble interrupts, if all critucal_enter have been removed
  * @param None  
  * @retval None
  */
void critical_exit(void);

/**
  * @brief Set the timer, stat it, and configure it to tick at 1ms.
  * @param None  
  * @retval None
  */
void tick_init(void);

/**
  * @brief Get the timer counter
  * @param None  
  * @retval Timer on 64 bits
  */
uint64_t tick_get(void);

#endif