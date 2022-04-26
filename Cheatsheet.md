# Setups

```c
#include <avr/io.h> // AVR register variables
#include <stdio.h> //
#include <stdint.h> // library for working with integers

#define F_CPU 2000000 // Need to define clock speed for delay.h
// Clock speed must be defined before delay.h is imported
#include <util/delay.h> // Time delay

sei(); //This special function call enables the use of interrupts
```

# PORTS
```c
DDRn = 0; // Set port to output
DDRn = 1; // Set port to input

PORTB |= (1<<PB5); // Turn on pin 5 of port B
PORTB &= ~(1<<PB5); // Turn off pin 5 of port B

_delay_ms(500); // Wait for 500ms

if (PINB & (1<<PB7)) { // Check if pinj 7 of port B is high
    // Code
}
```

# USART code
```c
#define PERIOD 0x2E // Some ASCII values
#define NEWLINE 0xA
#define RETURN 0xD

char string_buffer[32];
sprintf(string_buffer, "%s", "Hello World");
```

Number + 48 = ASCII code for that number

```c
void usart_init(uint16_t ubrr) {
	UCSR0B |= (1 << TXCIE0) | (1 << TXEN0);
	UBRR0 = ubrr;
}

void usart_transmit(uint8_t data) {
	while ((UCSR0A & (1 << UDRE0)) == 0) { } // While register is empty
	UDR0 = data;
}
```

# ADC Code

```c
void adc_init() {
	// Set reference voltage to AVcc
	ADMUX |= (1<<REFS0);
	// Set ADC Prescaler to 16 and turn on the ADC
	ADCSRA |= (1<<ADPS2) | (1<<ADEN);
	/*ADCSRB = 0;*/
}

uint16_t adc_read(uint8_t channel){
	// Set the channel to read from while not changing the other values in ADMUX
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	// Set ADSC to 1 to start the conversion
	ADCSRA |= (1<<ADSC);
	// Check ADSC to tell when the conversion finishes
	while( ADCSRA & (1<<ADSC) );
	return ADC;
}

uint16_t adc_convert_mv(uint16_t value){
	uint16_t result = ((uint32_t)value * 5000) / 1024;
	return result;
}

```

# Include file

```c
#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void adc_init();
uint16_t adc_read(uint8_t channel);
uint16_t adc_convert_mv(uint16_t value);

#endif /* ADC_H_ */
```
