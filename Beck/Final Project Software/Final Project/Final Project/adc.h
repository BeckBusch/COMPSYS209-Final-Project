#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void adc_init();
void adc_read(uint8_t channel);

uint16_t adc_calculate_mv();
uint16_t adc_calculate_ma();
uint16_t adc_calculate_pwr();

uint16_t adc_calculate_mv_pk();
uint16_t adc_calculate_ma_pk();

int16_t adc_calculate_mv_pt(uint16_t adcValue);
int16_t adc_calculate_ma_pt(uint16_t adcValue);

uint16_t roundNum(uint16_t value);

int32_t power;

#endif