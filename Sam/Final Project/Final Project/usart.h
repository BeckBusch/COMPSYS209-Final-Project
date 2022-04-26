#ifndef USART_H_
#define USART_H_

#include <stdint.h>

void usart_init(uint16_t ubrr);
void usart_transmit(uint8_t data);
void usart_number(uint16_t number, uint8_t decimal);
void usart_string(char[]);
void usart_printall();

#endif