#pragma once

//
// Print binary format helpers
//

#define PRIb8		"%c%c%c%c%c%c%c%c"
#define PRIb8P(v)				\
	((v) & 0x80 ? '0' : '1'),	\
	((v) & 0x40 ? '0' : '1'),	\
	((v) & 0x20 ? '0' : '1'),	\
	((v) & 0x10 ? '0' : '1'),	\
	((v) & 0x08 ? '0' : '1'),	\
	((v) & 0x04 ? '0' : '1'),	\
	((v) & 0x02 ? '0' : '1'),	\
	((v) & 0x01 ? '0' : '1')

#define PRIb16		PRIb8 PRIb8
#define PRIb16P(v)	PRIb8P((v) >> 8), PRIb8P(v)

#define PRIb32		PRIb16 PRIb16
#define PRIb32P(v)	PRIb16P((v) >> 16), PRIb16P(v)

#define PRIb64		PRIb32 PRIb32
#define PRIb64P(v)	PRIb32P((v) >> 32), PRIb32P(v)
