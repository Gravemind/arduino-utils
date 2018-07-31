
#include "RotaryEncoder.h"

// Arduino IDE does not support PCINT interrupt (!?), so pin
// numbers/registers are hard-coded all the way.
// Here, using pin 6 and 7 (PD6 PD7, PCINT22 PCINT23)

RotaryEncoder	rotenc;

uint8_t			g_processed_interrupt = 0;
int8_t			g_last_rotenc_value = 0;

void setup() {
	Serial.begin(9600);

	uint8_t		bv67 = _BV(6) | _BV(7);
	// pin as input
	DDRD &= ~bv67;
	PORTD &= ~bv67;
	// enable pin changed interrupt for PCINT22 and PCINT23
	PCICR |= _BV(PCIE2);
	PCMSK2 |= bv67;
}

void loop() {
	int8_t		rotenc_value = rotenc.value();
	if (g_last_rotenc_value != rotenc_value)
	{
		g_last_rotenc_value = rotenc_value;
		Serial.print("rot = ");
		Serial.println(g_last_rotenc_value);
	}
}

ISR(PCINT2_vect) {
	++g_processed_interrupt;
	const uint8_t	pins = PIND;
	rotenc.update((pins >> 6) & 1, (pins >> 7) & 1);
}
