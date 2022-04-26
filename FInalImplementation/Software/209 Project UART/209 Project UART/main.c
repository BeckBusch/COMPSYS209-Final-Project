/*
 * 209 Project UART.c
 *
 * Created: 27/08/2021 5:08:26 pm
 * Author : Beck Busch
 */ 

#define F_CPU 2000000UL

#include "headers.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>

#define PERIOD 0x2E
#define NEWLINE 0xA
#define RETURN 0xD

#define PeakVoltage 145
#define RMSCurrent 125
#define Power 160

int main(void) {
	// Initialization
	usart_init(12);
	uint8_t voltageText[17] = {80, 101, 97, 107, 32, 86, 111, 108, 116, 97, 103, 101, 32, 105, 115, 58, 32};
	uint8_t currentText[16] = {82, 77, 83, 32, 67, 117, 114, 114, 101, 110, 116, 32, 105, 115, 58, 32};
	uint8_t powerText[10] = {80, 111, 119, 101, 114, 32, 105, 115, 58, 32};
	
	// Preparation - can be moved inside the run loop when values are actually changing
	uint8_t voltageDigits[3]; uint8_t currentDigits[3]; uint8_t powerDigits[3];
	
	digits(PeakVoltage, voltageDigits);
	digits(RMSCurrent, currentDigits);
	digits(Power, powerDigits);
	
	// Run Loop
	while(1) {
		// Print Voltage label
		for (int i=0; i<17; i++){
			usart_transmit(voltageText[i]);
		}
		// Print Voltage digits
		usart_transmit(voltageDigits[0]+48);
		usart_transmit(voltageDigits[1]+48);
		usart_transmit(PERIOD);
		usart_transmit(voltageDigits[2]+48);
		// Print Newline
		usart_transmit(NEWLINE);
		usart_transmit(RETURN);
		
		// Print Current label
		for (int i=0; i<16; i++){
			usart_transmit(currentText[i]);
		}
		// Print Current digits
		for (int i=0; i<3; i++){
			usart_transmit(currentDigits[i]+48);
		}
		// Print Newline
		usart_transmit(NEWLINE);
		usart_transmit(RETURN);
		
		// Print Power label
		for (int i=0; i<10; i++){
			usart_transmit(powerText[i]);
		}
		// Print Power digits
		usart_transmit(powerDigits[0]+48);
		usart_transmit(PERIOD);
		usart_transmit(powerDigits[1]+48);
		usart_transmit(powerDigits[2]+48);
		// Print Newline
		usart_transmit(NEWLINE);
		usart_transmit(RETURN);
		
		// Print Newline
		usart_transmit(NEWLINE);
		usart_transmit(RETURN);
		
		// Wait for one second
		_delay_ms(1000);
	}
	
}