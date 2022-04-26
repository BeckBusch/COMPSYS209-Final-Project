#include "timer0.h"
#include "display.h"
#include "usart.h"

#include <avr/io.h>

void timer0_init(){
	
	//Setting CTC mode. Clear on Compare mode resets the counter when it matches OCR0A
	TCCR0A |= (1 << WGM01);
	
	//Setting prescaler to 256
	TCCR0B |= (1 << CS02);
	
	//Set OCR0A value to count 10 ms
	OCR0A = 77;
	
	//TIMSK0 |= (1 << OCIE0A);
}

void timer1_init(){
	
	// init timer 1 to trigger an interrupt once a second to run the measurement code
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	OCR1A = 31249; // :)
	TIMSK1 |= (1 << OCIE1A);
}
