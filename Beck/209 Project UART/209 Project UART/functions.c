#include "headers.h"

#include <avr/io.h>
#include <stdio.h>

void usart_init(uint16_t ubrr) {

	UCSR0B |= (1 << TXCIE0) | (1 << TXEN0);
	UBRR0 = ubrr;

}

void usart_transmit(uint8_t data) {

	while ((UCSR0A & (1 << UDRE0)) == 0) {

	}
	UDR0 = data;

}

void digits(uint8_t number, uint8_t *valueArray) {
	valueArray[0] = number / 100;
	valueArray[1] = (number / 10) % 10;
	valueArray[2] = number % 10;
}