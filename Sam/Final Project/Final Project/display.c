#include "display.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

#define Ds1 PD4
#define Ds2 PD5
#define Ds3 PD6
#define Ds4 PD7
#define SH_ST PC5
#define SH_DS PC4
#define SH_CP PC3

volatile uint16_t counter = 0;

//Array containing which segments to turn on to display a number between 0 to 9
const uint8_t segmentArray[10] = {0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x67};
const uint8_t unitArray[3] = {0x3E, 0x39, 0x73};

//4 characters to be displayed on Ds1 to Ds 4
static volatile uint8_t disp_characters[4] = {0,0,0,0};

//The current digit (e.g. the 1's, the 10's) of the 4-digit number we're displaying
static volatile uint8_t disp_position = 0;

void display_init() {
	// SH_ST, SH_DS, SH_CP as outputs
	DDRC |= (1 << SH_ST) | (1 << SH_DS) | (1 << SH_CP);
	// Ds1, Ds2, Ds3, Ds4 as outputs
	DDRD |= (1 << Ds1) | (1 << Ds2) | (1 << Ds3) | (1 << Ds4);
}

//Populate the array ‘disp_characters[]’ by separating the four digits of ‘number’
void separate_and_load_characters(uint16_t number, uint8_t decimal_pos, uint8_t letter){	
	//disp_characters[0] = segmentArray[number / 1000];
	disp_characters[0] = segmentArray[(number / 100) % 10];
	disp_characters[1] = segmentArray[(number / 10) % 10];
	disp_characters[2] = segmentArray[number % 10];

	//3. For the project you may modify this pattern to add decimal point at
	// the position ‘decimal_pos’
	disp_characters[decimal_pos-2] |= (1 << 7);
	
	// Display the correct unit
	disp_characters[3] = unitArray[letter];
}

//Render a single digit from ‘disp_characters[]’ on the display at ‘disp_positon’
void send_next_character_to_display(){
	uint8_t currentDigit = disp_characters[disp_position];
	
	//2. Send this bit pattern to the shift-register as in Q2.2
	// Turn off SH_CP and SH_ST
	PORTC &= ~((1 << SH_CP) | (1 << SH_ST));
	
	// Send each digit to the shift reg
	for (int i = 7; i >= 0; i--) {
		uint8_t currentBit = (currentDigit & (1 << i)) >> i;
		if (currentBit == 0) {
			PORTC &= ~(1 << SH_DS);
			} else {
			PORTC |= (1 << SH_DS);
		}
		
		// Shift the reg
		PORTC |= (1 << SH_CP);
		PORTC &= ~(1 << SH_CP);
	}
	
	//3. Disable all digits
	PORTD |= (1 << Ds1) | (1 << Ds2) | (1 << Ds3) | (1 << Ds4);
	
	//4. Latch the output by toggling SH_ST pin as in Q2.2
	// latch the output
	PORTC |= (1 << SH_ST);
	PORTC &= ~(1 << SH_ST);
	
	//5. Now, depending on the value of pos, enable the correct digit
	// (i.e. set Ds1, Ds2, Ds3 and Ds4 appropriately)
	switch (disp_position) {
		case 0:
			PORTD &= ~(1 << Ds1);
			break;
		case 1:
			PORTD &= ~(1 << Ds2);
			break;
		case 2:
			PORTD &= ~(1 << Ds3);
			break;
		case 3:
			PORTD &= ~(1 << Ds4);
			break;
	}
	
	//6. Increment ‘disp_position’ so the next of the 4 digits will be displayed
	// when function is called again from ISR (reset ‘disp_position’ after 3)
	if (disp_position > 4) {
		disp_position = 0;
	} else {
		disp_position++;
	}
}

