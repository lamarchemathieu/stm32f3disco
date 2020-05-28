#include "serial.h"

#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"

#include "fifo.h"
#include "utils.h"

#define FIFO_SIZE (1024+1)  //size of Fifo : +1 for the end of the buffer

//array use to 

static const char _hex_tab[] ="0123456789ABCDEF"; //array to translate hex code to caracters

void USART_CharReception_Callback(void);

//Fifo variables :
static uint8_t _buffer_rx[FIFO_SIZE];
static fifo_t _fifo_rx;

static uint8_t _buffer_tx[FIFO_SIZE];
static fifo_t _fifo_tx;

static volatile uint32_t _flg_tx_sending;



/**
  * @brief  Init of serioal.
  * @rmtoll BRR          BRy           LL_GPIO_ResetOutputPin
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:

  * @retval None
  */
void serial_init(void)
{

	//fifo init:
	fifo_init(&_fifo_rx, _buffer_rx, FIFO_SIZE);
	fifo_init(&_fifo_tx, _buffer_tx, FIFO_SIZE);
	_flg_tx_sending = 0;


	//Cfg of UART : 
	/*Using USAT1.
		USART1 is on APB2 
		Alternate function 7 (AF7) will set PA9 and PA10 as Tx and Rx.
	*/
	//Enable clk on GPIOA :
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	//Pin configuration
	LL_GPIO_InitTypeDef gpios; 	//Declare struct to set pin configuration
	LL_GPIO_StructInit(&gpios);	//I set a default value for my configuration

	gpios.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;	//Select pin 9 + 10
	gpios.Mode = LL_GPIO_MODE_ALTERNATE;		//Set to alternate function
	gpios.Alternate = LL_GPIO_AF_7;				//Alternate Function 7 selected
	gpios.OutputType = LL_GPIO_OUTPUT_PUSHPULL;	//The exit will be on push pull
	gpios.Pull = LL_GPIO_PULL_NO;				//No pull up / down on entry => High Z.

	LL_GPIO_Init(GPIOA, &gpios);	//Set configuration in port A

	//Config of USART1 :
	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);	//set USART to SystemClk (actually 64Mhz).
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1); 		//Activate clk of UART1

	//Config of UART : 115200, N, 8, 1 and oversampling : 16
	LL_USART_InitTypeDef usart;
	usart.BaudRate = 115200;
	usart.Parity = LL_USART_PARITY_NONE;
	usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	usart.OverSampling = LL_USART_OVERSAMPLING_16;
	usart.TransferDirection = LL_USART_DIRECTION_TX_RX;
	usart.StopBits = LL_USART_STOPBITS_1;
	usart.DataWidth = LL_USART_DATAWIDTH_8B;

	LL_USART_Init(USART1, &usart);
	//Mise en route de l'UART:
	LL_USART_Enable(USART1);

	//Enable interrupt :
	LL_USART_EnableIT_RXNE(USART1);
	LL_USART_EnableIT_TC(USART1);

	NVIC_EnableIRQ(USART1_IRQn);		//Interrupt on received data

}


void serial_print(const char* str)
{
	uint32_t index = 0;
	while (str[index] != '\0')
	{
		serial_write(str[index], 1);
		index ++;
	}
}


void serial_print_hex64(uint64_t value)
{
	serial_print ("0x");
	serial_write(_hex_tab[(value >> 60) & 0xF], 1);
	serial_write(_hex_tab[(value >> 56) & 0xF], 1);
	serial_write(_hex_tab[(value >> 52) & 0xF], 1);
	serial_write(_hex_tab[(value >> 48) & 0xF], 1);
	serial_write(_hex_tab[(value >> 44) & 0xF], 1);
	serial_write(_hex_tab[(value >> 40) & 0xF], 1);
	serial_write(_hex_tab[(value >> 36) & 0xF], 1);
	serial_write(_hex_tab[(value >> 32) & 0xF], 1);
	serial_write(_hex_tab[(value >> 28) & 0xF], 1);
	serial_write(_hex_tab[(value >> 24) & 0xF], 1);
	serial_write(_hex_tab[(value >> 20) & 0xF], 1);
	serial_write(_hex_tab[(value >> 16) & 0xF], 1);
	serial_write(_hex_tab[(value >> 12) & 0xF], 1);
	serial_write(_hex_tab[(value >> 8 ) & 0xF], 1);
	serial_write(_hex_tab[(value >> 4 ) & 0xF], 1);
	serial_write(_hex_tab[(value >> 0 ) & 0xF], 1);
}


void serial_print_hex32(uint32_t value)
{
	serial_print ("0x");
	serial_write(_hex_tab[(value >> 28) & 0xF], 1);
	serial_write(_hex_tab[(value >> 24) & 0xF], 1);
	serial_write(_hex_tab[(value >> 20) & 0xF], 1);
	serial_write(_hex_tab[(value >> 16) & 0xF], 1);
	serial_write(_hex_tab[(value >> 12) & 0xF], 1);
	serial_write(_hex_tab[(value >> 8 ) & 0xF], 1);
	serial_write(_hex_tab[(value >> 4 ) & 0xF], 1);
	serial_write(_hex_tab[(value >> 0 ) & 0xF], 1);
}


void serial_print_hex16(uint16_t value)
{
	serial_print ("0x");
	serial_write(_hex_tab[(value >> 12) & 0xF], 1);
	serial_write(_hex_tab[(value >> 8 ) & 0xF], 1);
	serial_write(_hex_tab[(value >> 4 ) & 0xF], 1);
	serial_write(_hex_tab[(value >> 0 ) & 0xF], 1);
}


void serial_print_hex8(uint8_t value)
{
	serial_print ("0x");
	serial_write(_hex_tab[(value >> 4) & 0xF], 1);
	serial_write(_hex_tab[(value >> 0) & 0xF], 1);
}


void USART1_IRQHandler(void)
{
	uint8_t data_to_transmit;
	
	if(LL_USART_IsActiveFlag_RXNE(USART1))
  	{
  		uint8_t c = LL_USART_ReceiveData8(USART1);
  		uint8_t tmp;
  		//if fifo is full
  		if (fifo_full(&_fifo_rx))
  		{
  			// pop of 1 char => we lost 1 char.
  			fifo_pop(&_fifo_rx, &tmp);
  		}
  		fifo_push(&_fifo_rx, c);
  	}



  	if (LL_USART_IsActiveFlag_TC(USART1))
  	{
  		LL_USART_ClearFlag_TC(USART1);
  		
  		if(fifo_pop(&_fifo_tx, &data_to_transmit))
  		{
			LL_USART_TransmitData8(USART1, data_to_transmit);
  		}
  		else
  		{
  			_flg_tx_sending = 0;
  		}
  	}		
}


uint32_t serial_available(void) //return if there is data to read.
{
	return !fifo_empty(&_fifo_rx);
}


uint8_t serial_read(void) //return 1 element from FIFO.
{
	uint8_t c = 0xff;
	fifo_pop(&_fifo_rx, &c);
	return c;
}


/*
*	data : data to store
* 	wait : who mutch time I have to push the data? 
*/
uint32_t serial_write(uint8_t data, uint32_t wait)
{
	uint8_t data_to_transmit;

	if (wait)
	{
		while(fifo_full(&_fifo_tx))
		{
		
		}
	}

	if (fifo_push(&_fifo_tx, data))
	{
		if (_flg_tx_sending)
		{
			//I'm sending => I keep sending.
		}
		else
		{
			//I have to push the first data
			fifo_pop(&_fifo_tx, &data_to_transmit);
			_flg_tx_sending = 1;
			LL_USART_TransmitData8(USART1, data_to_transmit);
		}
		return 1;
	}
	return 0;
}

void serial_print_bin32(uint32_t v)
{
	int32_t i;
	serial_print("0b");
	for(i = 31; i >= 0  ; i--)
	{
		serial_write(((v >> i) & 0x01) + '0', 1);
	}
}

void serial_print_bin16(uint16_t v)
{
	int32_t i;
	serial_print("0b");
	for(i = 15; i >= 0  ; i--)
	{
		serial_write(((v >> i) & 0x01) + '0', 1);
	}
}

void serial_print_bin8(uint8_t v)
{
	int32_t i;
	serial_print("0b");
	for(i = 7; i >= 0  ; i--)
	{
		serial_write(((v >> i) & 0x01) + '0', 1);
	}
}

void serial_print_dec(int32_t v)
{
	uint32_t started = 0;
	uint32_t i = 0;
	int32_t div = 1000000000;

	if (v == 0)
		serial_write('0', 1);

	if (v < 0)
	{
		serial_write('-', 1);
		v = -v;
	}

	for(i=0; i<10; i++)	
	{
		uint32_t tmp = v / div;

		v -= tmp * div;
		div /= 10;

		if ((tmp != 0) || (started))
		{
			started = 1;
			serial_write('0' + tmp, 1);
		}
	}
}