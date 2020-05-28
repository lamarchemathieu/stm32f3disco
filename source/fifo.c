#include "fifo.h"
#include "utils.h"



void fifo_init(fifo_t *fifo, uint8_t *buffer, uint32_t size)
{

	fifo->_fifo_head = 0; 			//store the head of the buffer
	fifo->_fifo_tail = 0;			//store the tail of the buffer
	fifo->_fifo_buffer = buffer;	//store the pointer on the buffer
	fifo->_fifo_size = size;		//store the size of the buffer
}

uint32_t fifo_push(fifo_t *fifo, uint8_t value)
{
	uint32_t return_value =0;

	//Disable IRQ :
	critical_enter();

	//if buffer is full => return
	if (fifo_full(fifo))
	{
		return_value = 0;
	}
	else
	{
		//if everything is ok, writing data on buffer + record of position.
		fifo->_fifo_buffer[fifo->_fifo_head] = value;
		fifo->_fifo_head = (fifo->_fifo_head + 1) % fifo->_fifo_size;
		return_value = 1;
	}

	//Enable IRQ
	critical_exit();

	return return_value;
}


uint32_t fifo_pop(fifo_t *fifo, uint8_t *value)
{
	uint32_t return_value =0;

	//Disable IRQ :
	critical_enter();

	//check if I have somthing to read : 
	if (fifo_empty(fifo))
	{
		return_value = 0 ;
	}
	else
	{
		//get the last data form buffer
		*value = fifo->_fifo_buffer[fifo->_fifo_tail];
		fifo->_fifo_tail = (fifo->_fifo_tail + 1) % fifo->_fifo_size;
		return_value = 1;
	}

	//Enable IRQ
	critical_exit();

	return return_value;
}


uint32_t fifo_full(fifo_t *fifo)
{
	uint32_t return_value = 0;

	//Disable IRQ :
	critical_enter();

	 return_value = (((fifo->_fifo_head+1) % fifo->_fifo_size) == fifo->_fifo_tail);
	/* 
	If at head + 1 I have tail => the buffer is full.
	The modulo on size of buffer handeling the warp.
	*/

	//Enable IRQ
	critical_exit();

	return return_value;
}


uint32_t fifo_empty(fifo_t *fifo)
{
	uint32_t return_value = 0;

	//Disable IRQ :
	critical_enter();

	return_value = fifo->_fifo_head == fifo->_fifo_tail;

	//Enable IRQ
	critical_exit();

	return return_value;
}
