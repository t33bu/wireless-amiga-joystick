/*
 * analog_diy.c
 *
 * Created on: 21.10.2017
 * Author: Teemu Leppänen (tjlepp@gmail.com)
 *
 * This work is licensed under Creative Commons Attribution-NonCommercial-ShareAlike (CC BY-NC-SA 3.0)
 * https://creativecommons.org/licenses/by-nc-sa/3.0/
 * 
 */

#include <avr/io.h>
#include <util/delay.h>

// **********************************
// Constants

// ADC limits
#define IDLE	127
#define UP		192
#define DOWN	64
#define LEFT	192
#define RIGHT	64

// Joystick pins
#define J_UP	PA2
#define J_DOWN	PA3
#define J_LEFT	PA4
#define J_RIGHT	PA5

// **********************************
// Function prototypes

void pin_init();
void adc_init();
void adc_read(void);

// **********************************
// Global variables

uint16_t vert_value = IDLE;
uint16_t hori_value = IDLE;

// **********************************
// Function implementations

int main() {

	pin_init();
	adc_init();

	while (1) {

		// ~500Hz
		_delay_ms(2);

		adc_read();

		if (vert_value > UP) {

			PORTA &= ~(1 << J_UP);
			PORTA |= (1 << J_DOWN);

		} else if (vert_value < DOWN) {

			PORTA &= ~(1 << J_DOWN);
			PORTA |= (1 << J_UP);

		} else {

			PORTA |= (1 << J_UP);
			PORTA |= (1 << J_DOWN);
		}

		vert_value = IDLE;

		if (hori_value < RIGHT) {

			PORTA &= ~(1 << J_RIGHT);
			PORTA |= (1 << J_LEFT);

		} else if (hori_value > LEFT) {

			PORTA &= ~(1 << J_LEFT);
			PORTA |= (1 << J_RIGHT);

		} else {

			PORTA |= (1 << J_RIGHT);
			PORTA |= (1 << J_LEFT);
		}

		hori_value = IDLE;
	}

	return 0;
}

// **********************************
void adc_init() {

	// Clear ADC reference voltage bits
	ADMUXB &= 0x1F;
	// Use external reference voltage (AREF pin)
	ADMUXB |= (1 << REFS2);

	// Enable ADC, set prescaler to 8kHz (1MHz / 128)
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// Use only high byte, i.e. ADC is now 8-bit
	ADCSRB |= (1 << ADLAR);
}

// **********************************
void adc_read() {

	// Select channel 6
	ADMUXA &= ~(1 << MUX3);
	ADMUXA |= (1 << MUX2) | (1 << MUX1);
	// Start single conversion
	ADCSRA |= (1 << ADSC);
	// Wait for conversion, ADSC = 0
	while(ADCSRA & (1 << ADSC));
	// Read value
	vert_value = ADCH;

	// Select channel 8
	ADMUXA &= ~(1 << MUX2);
	ADMUXA &= ~(1 << MUX1);
	ADMUXA |= (1 << MUX3);
	// Start single conversion
	ADCSRA |= (1 << ADSC);
	// Wait for conversion, ADSC = 0
	while(ADCSRA & (1 << ADSC));
	// Read value
	hori_value = ADCH;

}

// **********************************
void pin_init() {

	// Set joystick pins as output
	DDRA |= (1 << J_UP) | (1 << J_DOWN) | (1 << J_LEFT)| (1 << J_RIGHT);

	// Set to VCC by default
	PORTA |= (1 << J_UP);
	PORTA |= (1 << J_DOWN);
	PORTA |= (1 << J_LEFT);
	PORTA |= (1 << J_RIGHT);
}
