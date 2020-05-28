#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>

typedef struct 
{
 	uint8_t *_fifo_buffer;			//pointer on buffer
	volatile uint32_t _fifo_head;	//store the position of the head of buffer
	volatile uint32_t _fifo_tail;	//store the position of the tail of buffer
	uint32_t _fifo_size;			//store the size of the buffer
} fifo_t;


/**
  * @brief Init of the buffer
  * @param *fifo : pointer on structure
  *        *buffer : pointer on an array
  *        size : size of the array  
  * @retval None
  */
void fifo_init(fifo_t *fifo, uint8_t *buffer, uint32_t size);

/**
  * @brief Upsh data in fifo buffer.
  * @param *fifo : pointer on structure
  *        value : data to push on the structure
  * @retval 0 if push KO, 1 for a push ok.
  */
uint32_t fifo_push(fifo_t *fifo, uint8_t value);

/**
  * @brief Get data form fifo buffer.
  * @param *fifo : pointer on structure
  *        value : return the data by this pointer
  * @retval 0 if readin is ok.
  */
uint32_t fifo_pop(fifo_t *fifo, uint8_t *value);

/**
  * @brief If the circular fifo buffer is full or not.
  * @param *fifo : pointer on structure
  * @retval 1 if the buffer is full
  */
uint32_t fifo_full(fifo_t *fifo); 		//return 1 if buffer full

/**
  * @brief Tell you if the circular fifo buffer is empty.
  * @param *fifo : pointer on structure
  * @retval 1 if the buffer is empty.
  */
uint32_t fifo_empty(fifo_t *fifo); 		//return 1 if buffer is empty


#endif 