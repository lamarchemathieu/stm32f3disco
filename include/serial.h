
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

/**
  * @brief  Init of serial RS252.
  * @retval None
  */
void serial_init(void);

/**
  * @brief  Send data on serial.
  * @param Pointer on string to send. 
  * @retval None
  */
void serial_print(const char *str);

/**
  * @brief Send on serial and hex in ascii fromat 0xAF1221FA => "0xAF1221FA"
  * @param value to send (hex format). 
  * @retval None
  */
void serial_print_hex64(uint64_t value);

/**
  * @brief Send on serial and hex in ascii fromat 0xAF1221FA => "0xAF1221FA"
  * @param value to send (hex format). 
  * @retval None
  */
void serial_print_hex32(uint32_t value);

/**
  * @brief Send on serial and hex in ascii fromat 0xFA23 => "0xFA23"
  * @param value to send (hex format). 
  * @retval None
  */
void serial_print_hex16(uint16_t value);

/**
  * @brief Send on serial and hex in ascii fromat 0x1A => "0x1A"
  * @param value to send (hex format). 
  * @retval None
  */
void serial_print_hex8(uint8_t value);


void serial_print_bin32(uint32_t v);
void serial_print_bin16(uint16_t v);
void serial_print_bin8(uint8_t v);
void serial_print_dec(int32_t v);


/**
  * @brief Check if there's data to read on serial
  * @param None 
  * @retval 0 if no data >=1 if tere's some
  */
uint32_t serial_available(void);

/**
  * @brief Get one element form serial
  * @param None 
  * @retval A bite form the circular buffer FIFO.
  */
uint8_t serial_read(void);


/**
  * @brief Send data on serial.
  * @param data : The data to send
  *        wait : How mutch time I have to transmite the data before drop.
  * @retval 1 if send ok. 0 if send KO.
  */
uint32_t serial_write(uint8_t data, uint32_t wait);



#endif