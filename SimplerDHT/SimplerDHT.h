#pragma once

#include <stdint.h>

class SimplerDHT22 {
public:
	enum {
		NO_ERR = 0,
		ERR_CHECKSUM,
		ERR_TIMEOUT_INIT0,
		ERR_TIMEOUT_INIT1,
		ERR_TIMEOUT_INIT2,
		ERR_TIMEOUT_DATA0,
		ERR_TIMEOUT_DATA1,
	};

	// read result output
	uint16_t	_temp;
	uint16_t	_humi;

	// Read result to _temp and _humi. With fullPrecision, _temp and _humi are
	// x10. Without fullPrecision, they are rounded (x1).
	// Returns error code below.
	uint8_t		read(int pin, bool fullPrecision);

	// debug
	//uint8_t read_timings_i;
	//uint8_t read_timings[40];

protected:
	bool	wait_for(uint8_t targetv, uint8_t timeout);

	uint8_t _bytes[5];
	uint8_t	_port;
	uint8_t	_bit;
};

// implemented as weak, can be re-implemented elsewhere
void SimplerDHT_delayMicroseconds(uint8_t d);
void SimplerDHT_delay(uint8_t d);
