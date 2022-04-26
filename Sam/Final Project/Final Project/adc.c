#include <avr/io.h>

#include "adc.h"
#include "main.h"
#include "usart.h"

#define SAMPLESIZE 50

void adc_init() {
	
	// Set reference voltage to AVcc.
	ADMUX |= (1 << REFS0);
	
	// Set ADC Prescaler to 16, enable adc interrupt, enable free running trigger, and turn on the ADC.
	ADCSRA |= (1 << ADPS2) | (1 << ADIE) | (1 << ADATE) | (1 << ADEN);
}

void adc_read(uint8_t channel) {
	
	// Select the ADC channel without modifying the other bits.
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADEN);
	
	// Set the adc to record either voltage or current.
	currentRead = channel;
	
	// Start a conversion and wait for it to finish.
	ADCSRA |= (1 << ADSC);
}

uint16_t adc_calculate_mv_pk() {
	
	// Square each value and add them together.
	uint32_t pkVoltage = 0;
		for (uint8_t i = 0; i < SAMPLESIZE; i++) {
			pkVoltage += voltageValues[i] * voltageValues[i];
		}

	// Divide to get the average of the squares.
	pkVoltage /= SAMPLESIZE;

	// Start with an estimate of half, then iterate to get the square root.
	uint32_t sqrtV = pkVoltage / 2;	
	for (int j = 0; j < 10; j++) {
		sqrtV = ((sqrtV + pkVoltage / sqrtV) >> 1);
	}
	
	pkVoltage = sqrtV + 431; // Add the ADC equivalent of the offset (plus one).
	pkVoltage *= 5000; // Multiply by voltage reference.
	pkVoltage /= 1024; // Divide by maximum ADC value.
	pkVoltage -= 2100; // Subtract offset voltage.
	pkVoltage *= 1540; // Multiply by maximum RMS voltage.
	pkVoltage /= 1000; // Scale down.

	return (uint16_t)pkVoltage;	
}

uint16_t adc_calculate_mv() {

	// Find the maximum ADC reading for the voltage.
	uint32_t rmsVoltage = 0;
	for (uint8_t i = 0; i < SAMPLESIZE; i++) {
		if (voltageValues[i] > rmsVoltage) {
			rmsVoltage = voltageValues[i];
		}
	}
	
	rmsVoltage += 1; // Increment by 1 to change range to 1 to 1024.
	rmsVoltage *= 5000; // Multiply by voltage reference.
	rmsVoltage /= 1024; // Divide by maximum ADC value.
	rmsVoltage -= 2100; // Subtract offset voltage.
	rmsVoltage *= 1540; // Multiply by maximum RMS voltage.
	rmsVoltage /= 10; // Scale down.

	return (uint16_t)rmsVoltage;
}

int16_t adc_calculate_mv_pt(uint16_t adcValue) {
	
	// Cast voltage value to 32 bit int for calculation.
	int32_t voltageValue = adcValue;
	
	voltageValue += 1; // Increment by 1 to change range to 1 to 1024.
	voltageValue *= 5000; // Multiply by voltage reference.
	voltageValue /= 1024; // Divide by maximum ADC value.
	voltageValue -= 2100; // Subtract offset voltage.
	voltageValue *= 1540; // Multiply by maximum RMS voltage.
	voltageValue /= 100; // Scale down.

	return (int16_t)voltageValue;
}

uint16_t adc_calculate_ma_pk() {

	// Square each value and add them together.
	uint32_t pkCurrent = 0;
	for (uint8_t i = 0; i < SAMPLESIZE; i++) {
		pkCurrent += currentValues[i] * currentValues[i];
	}

	// Divide to get the average of the squares.
	pkCurrent /= SAMPLESIZE;

	// Start with an estimate of half, then iterate to get the square root.
	uint32_t sqrtV = pkCurrent / 2;
	for (int j = 0; j < 10; j++) {
		sqrtV = ((sqrtV + pkCurrent / sqrtV) >> 1);
	}
	
	pkCurrent = sqrtV + 431; // Add the ADC equivalent of the offset (plus one).
	pkCurrent *= 5000; // Multiply by voltage reference.
	pkCurrent /= 1024; // Divide by maximum ADC value.
	pkCurrent -= 2100; // Subtract offset voltage.
	pkCurrent *= 595; // Multiply by maximum RMS current.
	pkCurrent /= 10000; // Scale down.

	return (uint16_t)pkCurrent;
}

uint16_t adc_calculate_ma() {
	
	// Find the maximum ADC reading for the current.
	uint32_t rmsCurrent = 0;
	for (uint8_t i = 0; i < SAMPLESIZE; i++){
		if (currentValues[i] > rmsCurrent) {
			rmsCurrent = currentValues[i];
		}
	}
	rmsCurrent += 1; // Increment by 1 to change range to 1 to 1024.
	rmsCurrent *= 5000; // Multiply by voltage reference.
	rmsCurrent /= 1024; // Divide by maximum ADC value.
	rmsCurrent -= 2100; // Subtract offset voltage.
	rmsCurrent *= 595; // Multiply by maximum RMS current.
	rmsCurrent /= 100; // Scale down.

	return (uint16_t)rmsCurrent;
}

int16_t adc_calculate_ma_pt(uint16_t adcValue) {
	
	// Cast current value to 32 bit int for calculation.
	int32_t currentValue = adcValue;
	
	currentValue += 1; // Increment by 1 to change range to 1 to 1024.
	currentValue *= 5000; // Multiply by voltage reference.
	currentValue /= 1024; // Divide by maximum ADC value.
	currentValue -= 2100; // Subtract offset voltage.
	currentValue *= 595; // Multiply by maximum RMS current.
	currentValue /= 100; // Scale down.

	return (int16_t)currentValue;
}

uint16_t adc_calculate_pwr() {
	
	// Number of rectangles is one less than the number of samples in each array.
	int32_t power = 0;
	for (uint8_t i = 0; i < SAMPLESIZE - 1; i++) {

		// Take the average of each power value (the midpoint). Riemann summation.
		power += ((((int32_t)adc_calculate_mv_pt(voltageValues[i]) * adc_calculate_ma_pt(currentValues[i])) + \
		((int32_t)adc_calculate_mv_pt(voltageValues[i + 1]) * adc_calculate_ma_pt(currentValues[i + 1]))) >> 1);
		
	}

	power /= 5000; // Scale down and multiply by 2 to get correct power value.
	power /= (SAMPLESIZE - 1); // This is the midpoint method.
	
	/* If the power returns a negative magnitude, correct it as we know
	there is a positive power dissipation. */
	if (power < 0) {
		power *= -1;
	}
	
	return (uint16_t)power;
}

uint16_t roundNum(uint16_t value) {
	
	// Get the remainder.
	uint8_t rem = value % 10;
	
	// If we are rounding up, add the extra ten.
	if (rem >= 5) {
		value -= rem;
		value += 10;
	} else {
		value -= rem;
	}
	
	// Remove the extraneous zero.
	value /= 10;
	
	return value;
}

