#pragma once

#include <stdint.h>

//
// Advanced 2 pins rotary encoder decoder.
//
// Meant to be fast: could be run in an interrupt. And as reliable as possible:
// can handle half and slow stepping, and tries to fix "very fast rotation"
// (as-in "as fast as a human would want to rotate a rotary encoder")
//
// Internally, each pin change history is recorded in a single byte using a few
// simple bitwise operations. Then it looks for the full "rotation step encoded
// pattern" on both pins, so "half-steps" and "slow-stepping" should be handled
// properly. It can also debounce some contact bouncing. And tries to resolve
// "very fast rotation" ambiguities (both pins changing at same time).
//

// Enable the code that tries to handle contact bouncing
#ifndef ROTARY_ENCODER_DEBOUNCE
#	define ROTARY_ENCODER_DEBOUNCE		1
#endif

// Enable the code that tries to handles "very fast rotation"
// (when both pins changes at the same time)
#ifndef ROTARY_ENCODER_FAST_ROT
#	define ROTARY_ENCODER_FAST_ROT		1
#endif

// Enable debug
#ifndef DEBUG_ROTARY_ENCODER
#	define DEBUG_ROTARY_ENCODER		0
#endif

#if (DEBUG_ROTARY_ENCODER != 0)
#	define IF_DEBUG_ROTARY_ENCODER(...)	__VA_ARGS__
#else
#	define IF_DEBUG_ROTARY_ENCODER(...)
#endif

class RotaryEncoder
{
public:
	typedef int8_t		irot_t;
	typedef int8_t		ihist_t;
	typedef uint8_t		uhist_t;

	// `update()` and `value()` should be interrupt-safe as long as irot_t is a
	// register-size, and ++/-- is interrupt-atomic.

	// Evaluate new rotary encoder pin values and update `value` if necessary.
	void		update(bool pina, bool pinb);

	// Get the current rotation value.
	irot_t		value() const { return _value; }

	uhist_t		pinaHistoryBits() const { return _pina_hist; }
	uhist_t		pinbHistoryBits() const { return _pinb_hist; }

	IF_DEBUG_ROTARY_ENCODER(
		// Number of times "fast rotation" was detected (both pins changed at same ping)
		uint8_t	_fast = 0;
		// Number of times value was incremented during "fast rotation"
		uint8_t	_inc_fast = 0;	// at 1st change
		uint8_t	_inc_fast2 = 0; // at 2nd change
		// Number of times bouncing was detected during normal operation
		uint8_t	_bounce = 0;
		// Number of times bouncing was detected during "fast rotation"
		uint8_t	_bounce_fast = 0;
	)

private:
	irot_t		_value = 0;
	uhist_t		_pina_hist = ~0;
	uhist_t		_pinb_hist = ~0;
};

inline
void	RotaryEncoder::update(bool pina, bool pinb)
{
	bool	prev_pina = (_pina_hist) & 1;
	bool	prev_pinb = (_pinb_hist) & 1;

#if (ROTARY_ENCODER_FAST_ROT != 0)
	// if both pins changed at same time ("very fast rotation")
	// fake a change with only 1 pin change at a time
	bool	fast_rot = (pina != prev_pina && pinb != prev_pinb);
	IF_DEBUG_ROTARY_ENCODER(bool debug_fast = fast_rot;)
	if (fast_rot)
	{
		IF_DEBUG_ROTARY_ENCODER(++_fast;)

		// if the last change was pinb, record pina change,
		// and vice versa
		uint8_t	prev_prev_pinb = ((_pinb_hist >> 1) & 1);
		if (prev_pinb != prev_prev_pinb)
		{
			_pina_hist <<= 1;
			_pina_hist |= uhist_t(pina);
			_pinb_hist <<= 1;
			_pinb_hist |= uhist_t(prev_pinb); // (re-record same value)
		}
		else
		{
			_pinb_hist <<= 1;
			_pinb_hist |= uhist_t(pinb);
			_pina_hist <<= 1;
			_pina_hist |= uhist_t(prev_pina); // (re-record same value)
		}

		// Go check this first change
		goto check_last_change;
	}
#endif // (ROTARY_ENCODER_FAST_ROT != 0)

	// one of the two pin changed
	if (pina != prev_pina || pinb != prev_pinb)
	{

#if (ROTARY_ENCODER_FAST_ROT != 0)
	record_new_change:
#endif
		_pina_hist <<= 1;
		_pina_hist |= uhist_t(pina);
		_pinb_hist <<= 1;
		_pinb_hist |= uhist_t(pinb);

#if (ROTARY_ENCODER_FAST_ROT != 0)
	check_last_change:
#endif
#if (ROTARY_ENCODER_DEBOUNCE != 0)
		// detect bounce on the last 3 pin states
		const uint8_t	last_3_pina = (_pina_hist & 0b111);
		const uint8_t	last_3_pinb = (_pinb_hist & 0b111);
		if (((last_3_pina == 0b000) && (last_3_pinb == 0b010)) ||
			((last_3_pinb == 0b000) && (last_3_pina == 0b010)))
		{
			IF_DEBUG_ROTARY_ENCODER(if (debug_fast) ++_bounce_fast; else ++_bounce;)
			// remove last 2 states from history
			// (signed arithmetic shift to repeat the last bit/state)
			reinterpret_cast<ihist_t&>(_pina_hist) >>= 2;
			reinterpret_cast<ihist_t&>(_pinb_hist) >>= 2;
		}
		else
#endif
		{
			// test for inc/dec pattern on last 5 pin states
			if ((_pina_hist & 0b11111) == 0b10011 &&
				(_pinb_hist & 0b11111) == 0b11001)
			{
				IF_DEBUG_ROTARY_ENCODER(if (debug_fast) { if (fast_rot) ++_inc_fast; else ++_inc_fast2; })
				++_value;
			}
			else if ((_pinb_hist & 0b11111) == 0b10011 &&
					 (_pina_hist & 0b11111) == 0b11001)
			{
				IF_DEBUG_ROTARY_ENCODER(if (debug_fast) { if (fast_rot) ++_inc_fast; else ++_inc_fast2; })
				--_value;
			}
		}
	}

#if (ROTARY_ENCODER_FAST_ROT != 0)
	if (fast_rot)
	{
		fast_rot = false;
		// Got record then check the second change
		goto record_new_change;
	}
#endif
}
