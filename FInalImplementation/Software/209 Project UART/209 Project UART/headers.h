#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdint.h>

void usart_init(uint16_t ubrr);
void usart_transmit(uint8_t data);
void digits(uint8_t number, uint8_t *valueArray);

#endif /* HEADERS_H_ */