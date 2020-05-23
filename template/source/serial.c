
#include "serial.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"


void serial_init(uint32_t baudrate)
{
	LL_GPIO_InitTypeDef gpios;

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_GPIO_StructInit(&gpios);
	gpios.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
	gpios.Mode = LL_GPIO_MODE_ALTERNATE;
	gpios.Pull = LL_GPIO_PULL_NO;
	gpios.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpios.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
	gpios.Alternate  = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &gpios);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
	LL_USART_InitTypeDef usart;
	LL_USART_StructInit(&usart);
	usart.BaudRate            = baudrate;
	usart.DataWidth           = LL_USART_DATAWIDTH_8B;
	usart.StopBits            = LL_USART_STOPBITS_1;
	usart.Parity              = LL_USART_PARITY_NONE;
	usart.TransferDirection   = LL_USART_DIRECTION_TX_RX;
	usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	usart.OverSampling        = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &usart);
	LL_USART_Enable(USART1);

	LL_USART_EnableIT_RXNE(USART1);

	NVIC_EnableIRQ(USART1_IRQn);
}

void serial_transmit(const uint8_t *data, uint32_t length)
{
	uint32_t i;
	for(i = 0; i < length; i++)
	{
		serial_put(data[i]);
	}
}

void serial_print(const char *str)
{
	uint32_t i=0;

	while(str[i])
	{
		serial_put(str[i]);
		i++;
	}
}

void serial_put(uint8_t c)
{
	while(LL_USART_IsActiveFlag_TC(USART1) == 0)
	{

	}
	LL_USART_TransmitData8(USART1, c);
}

void USART1_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_RXNE(USART1))
	{
		uint8_t c = LL_USART_ReceiveData8(USART1);
		LL_USART_TransmitData8(USART1, c);
		LL_GPIO_TogglePin(GPIOE, LL_GPIO_PIN_9);
	}
}

static const char _hex[] = "0123456789ABCDEF";

void serial_print_hex32(uint32_t v)
{
	serial_print("0x");
	serial_put(_hex[(v >> 28) & 0x0F]);
	serial_put(_hex[(v >> 24) & 0x0F]);
	serial_put(_hex[(v >> 20) & 0x0F]);
	serial_put(_hex[(v >> 16) & 0x0F]);
	serial_put(_hex[(v >> 12) & 0x0F]);
	serial_put(_hex[(v >>  8) & 0x0F]);
	serial_put(_hex[(v >>  4) & 0x0F]);
	serial_put(_hex[(v >>  0) & 0x0F]);
}

void serial_print_hex16(uint16_t v)
{
	serial_print("0x");
	serial_put(_hex[(v >> 12) & 0x0F]);
	serial_put(_hex[(v >>  8) & 0x0F]);
	serial_put(_hex[(v >>  4) & 0x0F]);
	serial_put(_hex[(v >>  0) & 0x0F]);
}

void serial_print_hex8(uint8_t v)
{
	serial_print("0x");
	serial_put(_hex[(v >>  4) & 0x0F]);
	serial_put(_hex[(v >>  0) & 0x0F]);
}

void serial_print_bin32(uint32_t v)
{
	int32_t i;
	serial_print("0b");
	for(i = 31; i >= 0  ; i--)
	{
		serial_put(((v >> i) & 0x01) + '0');
	}
}

void serial_print_bin16(uint16_t v)
{
	int32_t i;
	serial_print("0b");
	for(i = 15; i >= 0  ; i--)
	{
		serial_put(((v >> i) & 0x01) + '0');
	}
}

void serial_print_bin8(uint8_t v)
{
	int32_t i;
	serial_print("0b");
	for(i = 7; i >= 0  ; i--)
	{
		serial_put(((v >> i) & 0x01) + '0');
	}
}



void serial_print_dec(uint32_t v)
{
	uint32_t started = 0;
	uint32_t i = 0;
	uint32_t div = 1000000000;
	for(i=0; i<10; i++)	
	{
		uint32_t tmp = v / div;

		v -= tmp * div;
		div /= 10;

		if ((tmp != 0) || (started))
		{
			started = 1;
			serial_put('0' + tmp);
		}
	}
}