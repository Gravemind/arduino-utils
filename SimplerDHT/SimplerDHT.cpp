
#include "SimplerDHT.h"

#include <Arduino.h>

#include <util/atomic.h>

#if 0
static inline uint8_t micros8() {
	uint8_t t;

#if defined(TCNT0)
	t = TCNT0;
#elif defined(TCNT0L)
	t = TCNT0L;
#else
	#error TIMER 0 not defined
#endif

	unsigned long long	timer0_prescaler = 64; // arduino's init() default
	return (t) * (timer0_prescaler / clockCyclesPerMicrosecond());
}
#endif

uint8_t SimplerDHT22::read(int pin, bool fullPrecision)
{
	//memset(read_timings, sizeof(read_timings), 0);
	//read_timings_i = 0;

	_bit = digitalPinToBitMask(pin);
	_port = digitalPinToPort(pin);

	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);

	SimplerDHT_delay(1);

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		digitalWrite(pin, HIGH);
		pinMode(pin, INPUT_PULLUP);

		if (!wait_for(LOW, 200))
			return ERR_TIMEOUT_INIT0;

		if (!wait_for(HIGH, 85))
			return ERR_TIMEOUT_INIT1;

		if (!wait_for(LOW, 85))
			return ERR_TIMEOUT_INIT2;

		uint8_t lowtime = 4;
		for (uint8_t i = 0; i < sizeof(_bytes); ++i)
		{
			uint8_t byt = 0;
			for (uint8_t j = 0; j < 8; ++j)
			{
				lowtime = 0;
				while (!(*portInputRegister(_port) & _bit))
				{
					++lowtime;
					if (lowtime == 0)
						return ERR_TIMEOUT_DATA0;
				}
				byt <<= 1;
				//read_timings[read_timings_i++] = lowtime;
				uint8_t hitime = 0;
				while ((*portInputRegister(_port) & _bit))
				{
					++hitime;
					if (hitime == 0)
						return ERR_TIMEOUT_DATA1;
				}
				byt |= (hitime > lowtime);
				//read_timings[read_timings_i++] = hitime;
			}
			_bytes[i] = byt;
			//SimplerDHT_delayMicroseconds(0);
		}
	}

	uint8_t		checksum = _bytes[0] + _bytes[1] + _bytes[2] + _bytes[3];
	if (checksum != _bytes[4])
		return ERR_CHECKSUM;

	uint16_t	humi = ((uint16_t)(_bytes[0]) << 8) | _bytes[1];
	uint16_t	temp = ((uint16_t)(_bytes[2]) << 8) | _bytes[3];

	if (fullPrecision)
	{
		_humi = humi;
		_temp = temp;
	}
	else
	{
		// round
		_humi = (humi + 5) / 10;
		_temp = (temp + 5) / 10;
	}

	return NO_ERR;
}

bool	SimplerDHT22::wait_for(uint8_t targetv, uint8_t timeout)
{
	if (!!(*portInputRegister(_port) & _bit) == targetv)
		return true;
	uint8_t		sampling_interval = 10; // us
	uint8_t		t = 0;
	while (t < timeout)
	{
		t += sampling_interval;
		uint8_t	v = !!(*portInputRegister(_port) & _bit);
		if (v == targetv)
		{
			//read_timings[read_timings_i++] = t;
			return true;
		}
		//_delay_us(sampling_interval);
		SimplerDHT_delayMicroseconds(sampling_interval);
	}
	return true;
}

void __attribute__((weak)) SimplerDHT_delayMicroseconds(uint8_t d)
{
	delayMicroseconds(d);
}

void __attribute__((weak)) SimplerDHT_delay(uint8_t d)
{
	delay(d);
}
