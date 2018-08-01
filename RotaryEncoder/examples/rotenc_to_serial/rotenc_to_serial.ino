
#define DEBUG_ROTARY_ENCODER	0

#include <util/delay.h>

#include "RotaryEncoder.h"

#if (DEBUG_ROTARY_ENCODER != 0)
#	include "utils/printb.h"
#	include "utils/PrintFDev.h"
PrintFDevRef	stdserial(&Serial);
#endif

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
	// const uint8_t	pins = PIND;
	// rotenc.update((pins >> 6) & 1, (pins >> 7) & 1);
	//_delay_us(500);

	int8_t		rotenc_value = rotenc.value();
	if (g_last_rotenc_value != rotenc_value)
	{
		g_last_rotenc_value = rotenc_value;

#if (DEBUG_ROTARY_ENCODER != 0)
		fprintf(stdserial,
				"rot=%4d fast=%2d inc_fast=%2d inc_fast2=%2d bounce=%2d bounce_fast=%2d\n",
				g_last_rotenc_value,
				rotenc._fast,
				rotenc._inc_fast,
				rotenc._inc_fast2,
				rotenc._bounce,
				rotenc._bounce_fast);

		if (rotenc._inc_fast2 > 0)
		{
			fprintf(stdserial, PRIb8 "\n" PRIb8 "\n",
					PRIb8P(rotenc.pinaHistoryBits()),
					PRIb8P(rotenc.pinbHistoryBits()));
		}

#else
		Serial.print("rot = ");
		Serial.println(g_last_rotenc_value);
#endif
	}

}

ISR(PCINT2_vect) {
	++g_processed_interrupt;
	const uint8_t	pins = PIND;
	rotenc.update((pins >> 6) & 1, (pins >> 7) & 1);
}
