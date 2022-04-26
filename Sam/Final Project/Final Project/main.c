/*
 * Final Project.c
 *
 * Author : Team 29
 */ 

#define F_CPU 2000000UL

#define SAMPLESIZE 50

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

#include "main.h"
#include "timer0.h"
#include "adc.h"
#include "display.h"
#include "usart.h"

// 1 second timer to control program running
ISR(TIMER1_COMPA_vect){
	
	oneSecondCheck = 0;
}

ISR(TIMER0_COMPA_vect){
	
	send_next_character_to_display();
}

// Interrupt to tell the code when Vs is at 0V
ISR(INT0_vect) {
	
	zeroCrossing = 1;
}

ISR(ADC_vect) {
	
	 // Save ADC result to correct array
	if (currentRead) {
		currentValues[adcCounter] = ADC;
	} else {
		voltageValues[adcCounter] = ADC;
	}
	
	// Increment counter
	adcCounter++;
	if (adcCounter >= SAMPLESIZE) {
		ADCSRA &= ~(1 << ADEN);
	}
}

int main(void) {
	
	timer1_init();
	timer0_init();
	adc_init();
	usart_init(12);
	interrupt_init();
	display_init();
	sei();
	
	displayValue = 0;
			
	while (1) {
		
		// wait for Vs to be 0 volts before starting to record values
		zeroCrossing = 0;
		while (!zeroCrossing);
		
		// set adcCounter to 0 and start the adc reading voltage values
		adcCounter = 0;
		adc_read(0);
		
		// wait for the adc to be turned off after 50 conversions
		while (ADCSRA & (1 << ADEN));// { NOOP; }
		
		// wait for Vs to be 0 volts before starting to record values
		zeroCrossing = 0;
		while (!zeroCrossing);
		
		// set adcCounter to 0 and start the adc reading current values
		adcCounter = 0;
		adc_read(1);
		
		// wait for the adc to be turned off after 50 conversions
		while (ADCSRA & (1 << ADEN));// { NOOP; }
		
		// Calculate the required output numbers
		voltageOutput = roundNum(adc_calculate_mv_pk());
		currentOutput = roundNum(adc_calculate_ma());
		powerOutput = roundNum(adc_calculate_pwr());
		
		// Choose which value to display via 7 Segment Display
		// This alternates between voltage current and power every 1 second
		switch (displayValue) {
		case 0:
			separate_and_load_characters(voltageOutput, 3, 0);
			displayValue++;
			break;
		case 1:
			separate_and_load_characters(currentOutput, 0, 1);
			displayValue++;
			break;
		case 2:
			separate_and_load_characters(powerOutput, 2, 2);
			displayValue = 0;
			break;
		}
				
		// print all values to the usart terminal		
		usart_printall();
		
		// wait for the one second timer to finish before restarting the code
		// To avoid problems during the rest of the code,
		// the 7 Segment Display interrupt timer is only enabled during this wait period
		TIMSK0 |= (1 << OCIE0A);
		oneSecondCheck = 1;
		while (oneSecondCheck);
		TIMSK0 &= ~(1 << OCIE0A);
	}
}

void interrupt_init() {
	
	// Rising edge on INT0 generates an interrupt
	EICRA |= (1 << ISC01) | (1 << ISC00);
	
	// Enable INT0
	EIMSK |=  (1 << INT0);
}

