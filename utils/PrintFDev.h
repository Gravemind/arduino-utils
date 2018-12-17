#pragma once

#include <stdio.h>

//
// PrintFDevRef instances can be passed to stream write functions (via cast-opt
// to FILE*) to output on `_Parent *parent` (usually a `Print` class).
//
//	 PrintFDevRef	stdserial(&Serial);
//	 fprintf(stdserial, "%f\n", 42.1);
//
class PrintFDevRef
{
public:
#if defined(__AVR__)
	template <typename _Parent>
	PrintFDevRef(_Parent *parent)
	{
		fdev_setup_stream(
			&_stream,
			[](char c, FILE *f){
				_Parent	*self = (_Parent*)fdev_get_udata(f);
				//PrintFDev	*self = (PrintFDev*)((uint8_t*)f - offsetof(PrintFDev*, _stream));
				self->write(c);
				return 1;
			},
			nullptr,
			_FDEV_SETUP_WRITE
		);
		fdev_set_udata(&_stream, (void*)parent);
	}

	FILE		*stream() { return &_stream; }
	operator	FILE*() { return &_stream; }
	FILE		_stream;

#else // GNU stdio (man fopencookie)

	template <typename _Parent>
	PrintFDevRef(_Parent *parent)
	{
		cookie_io_functions_t  io_funcs = {
			// .read  = [](void *cookie, char *buf, size_t size) -> ssize_t {
			// 			 _Parent *parent = (_Parent*)cookie;
			// 			 return parent->readBytes(buf, size);
			// 		 },
			.read = nullptr,
			.write = [](void *cookie, const char *buf, size_t size) -> ssize_t {
						 _Parent *parent = (_Parent*)cookie;
						 return parent->write(buf, size);
					 },
			.seek  = nullptr,
			.close = nullptr
		};
		_stream = fopencookie(parent, "w", io_funcs);
		setlinebuf(_stream); // if not, requires explicit fflush !?
	}

	FILE		*stream() { return _stream; }
	operator	FILE*() { return _stream; }
	FILE		*_stream;
#endif
};

//
// Wraps a class `_Parent` (usually a `Print` class) so its instances can be
// passed to stream write functions (via cast-op FILE*).
//
//	 PrintFDev<Adafruit_SSD1306>	display;
//	 fprintf(display, "%f\n", 42.1);
//
template <typename _Parent>
class PrintFDev : public _Parent
{
public:
	PrintFDev() : _Parent(), _streamref(static_cast<_Parent*>(this)) { }
	FILE			*stream() { return _streamref.stream(); }
	operator		FILE*() { return _streamref.operator FILE *(); }
	PrintFDevRef	_streamref;
};
