#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

void display_init();
void separate_and_load_characters(uint16_t number, uint8_t decimal_pos, uint8_t letter);
void send_next_character_to_display();
extern volatile uint16_t counter;

extern volatile uint8_t usartNumber;

#endif