#pragma once

//
// Displays the bits of `bits` as a 0/1 graph
// Tested with _Display Adafruit_GFX.
//
template <typename _Display, typename _T>
inline void		draw_bits_as_graph(_Display &display, const _T &bits)
{
	const uint8_t	top_margin = 2;
	const uint8_t	width = 2;
	const uint8_t	height = 5;
	uint8_t			x = display.getCursorX();
	uint8_t			y = display.getCursorY();
	bool			last_bit = false;
	uint8_t			count = sizeof(_T) * 8;
	for (uint8_t i = 0; i < count; ++i)
	{
		//bool	curr_bit = bits & (_T(1) << (i));
		bool	curr_bit = bits & (_T(1) << (count - i - 1));
		if (i > 0 && curr_bit != last_bit)
			display.drawFastVLine(x + i * width, y + top_margin, height + 1, 1);
		last_bit = curr_bit;
		display.drawFastHLine(x + i * width, y + top_margin + (curr_bit ? 0 : height), width, 1);
	}
	display.setCursor(x + width * sizeof(_T) * 8, y);
}
