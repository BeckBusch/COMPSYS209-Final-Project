#include "usart.h"
#include "display.h"
#include "main.h"
#include "adc.h"

#include <avr/io.h>
#include <string.h>
#include <stdio.h>

#define NEWLINE 0x0A
#define RETURN 0x0D
#define PERIOD 0x2E

char transmitBuffer[32];

void usart_init(uint16_t ubrr) {
	
	UCSR0B |= (1 << TXEN0);
	UBRR0 = ubrr;
}

void usart_transmit(uint8_t data) {
	
	// Wait for transmit register to be empty
	while ((UCSR0A & (1 << UDRE0)) == 0) { }
	
	// Fill transmit regester with the data
	UDR0 = data;
}

void usart_number(uint16_t number, uint8_t decimal) {
	
	sprintf(transmitBuffer, "%u", number);
	
	int j = 0;
	if (decimal) {
		for (j = 0; j < decimal - 1; j++) {
			usart_transmit(transmitBuffer[j]);
		}
		usart_transmit(PERIOD);
	}
	
	while (transmitBuffer[j] != '\0') {
		usart_transmit(transmitBuffer[j++]);
	}
	
}

void usart_string(char string[]) {
	
	int j = 0;
	while (string[j] != '\0') {
		usart_transmit(string[j++]);
	}
	
}

void usart_printall() {
	
	usart_string("Peak voltage: ");
	usart_number(voltageOutput, 3); // Display Peak Voltage
	usart_string("V");
	usart_transmit(NEWLINE);
	usart_transmit(RETURN);
	
	usart_string("RMS current: ");
	usart_number(currentOutput, 0); // Display RMS Current
	usart_string("mA");
	usart_transmit(NEWLINE);
	usart_transmit(RETURN);
	
	usart_string("Real power: ");
	usart_number(powerOutput, 2); // Display Real Power
	usart_string("W");
	usart_transmit(NEWLINE);
	usart_transmit(RETURN);
	
	usart_transmit(NEWLINE);
	usart_transmit(RETURN);
}

