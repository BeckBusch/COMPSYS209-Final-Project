#ifndef MAIN_H_
#define MAIN_H_

#define SAMPLESIZE 50

#include <stdint.h>

void interrupt_init();

volatile uint16_t voltageValues[SAMPLESIZE];
volatile uint16_t currentValues[SAMPLESIZE];
volatile uint8_t adcCounter;
volatile uint8_t currentRead;
volatile uint8_t zeroCrossing;
volatile uint8_t oneSecondCheck;

uint16_t voltageOutput;
uint16_t currentOutput;
uint16_t powerOutput;

uint8_t displayValue;

#endif