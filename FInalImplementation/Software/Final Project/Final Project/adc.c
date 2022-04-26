#include <avr/io.h>

#include "adc.h"
#include "main.h"
#include "usart.h"

#define SAMPLESIZE 50

void adc_init() {
	// Set reference voltage to AVcc
	ADMUX |= (1 << REFS0);
	// Set ADC Prescaler to 16, enable adc interrupt, enable free running trigger, and turn on the ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADIE) | (1 << ADATE) | (1 << ADEN);
}

void adc_read(uint8_t channel) {
	// Select the ADC channel without modifying the other bits.
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADEN);
	
	// set the adc to record either voltage or current
	currentRead = channel;
	
	// Start a conversion and wait for it to finish.
	ADCSRA |= (1 << ADSC);
}

uint16_t adc_calculate_mv_pk() {

	uint32_t pkVoltage = 0;
	uint32_t rmsVoltage = (uint32_t)adc_calculate_mv();

	// Square the rms voltage, then subtract the square of the DC offset.
	pkVoltage = rmsVoltage * rmsVoltage;
	pkVoltage -= 44100;

	// Multiply by 2 as per the formula.
	pkVoltage *= 2;

	// Start with an estimate of half, then iterate to get the square root.
	uint32_t sqrtV = pkVoltage / 2;
	for (int j = 0; j < 20; j++) {
		sqrtV = ((sqrtV + pkVoltage / sqrtV) >> 1);
	}

	// Set the peak voltage value and return.
	pkVoltage = sqrtV;
	pkVoltage /= 10; // Scale down.
	return (uint16_t)pkVoltage;
}

uint16_t adc_calculate_mv() {

	uint32_t rmsVoltage = 0;
	for (uint8_t i = 0; i < SAMPLESIZE; i++){
		if (voltageValues[i] > rmsVoltage) {
			rmsVoltage = voltageValues[i];
		}
	}
	rmsVoltage *= 5000;
	rmsVoltage /= 1024;
	rmsVoltage -= 2100;
	rmsVoltage *= 1540;
	rmsVoltage /= 100;

	return (uint16_t)rmsVoltage;

}

int16_t adc_calculate_mv_pt(uint16_t adcValue) {
	int32_t voltageValue = adcValue;
	voltageValue *= 5000;
	voltageValue /= 1024;
	voltageValue -= 2100;
	voltageValue *= 1540;

	voltageValue /= 100;

	return (int16_t)voltageValue;
}

uint16_t adc_calculate_ma_pk() {

	uint32_t rmsCurrent = 0;
	for (uint8_t i = 0; i < SAMPLESIZE; i++){
		if (currentValues[i] > rmsCurrent) {
			rmsCurrent = currentValues[i];
		}
	}
	rmsCurrent *= 5000;
	rmsCurrent /= 1024;
	rmsCurrent -= 2100;//
	rmsCurrent *= 595;
	rmsCurrent *= 1414;
	rmsCurrent /= 100000;


	return (uint16_t)rmsCurrent;
}

uint16_t adc_calculate_ma() {
	uint32_t rmsCurrent = 0;
	for (uint8_t i = 0; i < SAMPLESIZE; i++){
		if (currentValues[i] > rmsCurrent) {
			rmsCurrent = currentValues[i];
		}
	}
	rmsCurrent *= 5000;
	rmsCurrent /= 1024;
	rmsCurrent -= 2100;//
	rmsCurrent *= 595;
	rmsCurrent /= 100;

	return (uint16_t)rmsCurrent;
}

int16_t adc_calculate_ma_pt(uint16_t adcValue) {
	int32_t currentValue = adcValue;//
	currentValue *= 5000;//
	currentValue /= 1024;//
	currentValue -= 2100;//
	currentValue *= 595;//
	currentValue /= 100;//

	return (int16_t)currentValue;

}

uint16_t adc_calculate_pwr() {
	power = 0;
	
	for (uint8_t i = 0; i < SAMPLESIZE - 1; i++) {

		power += ((((int32_t)adc_calculate_mv_pt(voltageValues[i]) * adc_calculate_ma_pt(currentValues[i])) + ((int32_t)adc_calculate_mv_pt(voltageValues[i + 1]) * adc_calculate_ma_pt(currentValues[i + 1]))) >> 1);
	}

	
	power /= 5000;
	power /= (SAMPLESIZE - 1); // MIDPOINT METHOD RIEMANN SUM
	// one less because if there are n points then there are n - 1 rectangles.
	if (power < 0) {
		power *= -1;
	}
	return (uint16_t)power;

}

uint16_t roundNum(uint16_t value) {
	uint8_t rem = value % 10;
	if (rem >= 5) {
		value -= rem;
		value += 10;
	} else {
		value -= rem;
	}
	value /= 10;
	return value;
}

