#include <stdint.h>
#include <assert.h>

#include "../compat/pgmspace.h"
#include "bitmapscroller.h"
#include "outofspec.h"


static uint8_t const aOutOfSpecBitmap [2][55][8] PROGMEM =
	{{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	  {0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00},
	  {0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
	  {0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
	  {0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
	  {0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
	  {0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
	  {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
	  {0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
	  {0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x42, 0xc0},
	  {0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x00},
	  {0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xc0},
	  {0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe2, 0x80},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe8, 0x00},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00},
	  {0x78, 0x98, 0x8d, 0x0b, 0x68, 0xff, 0xe8, 0x80},
	  {0x7a, 0xab, 0xaa, 0xdb, 0x6b, 0xff, 0xe8, 0x60},
	  {0x7a, 0x98, 0xaa, 0xdb, 0x28, 0xff, 0xe4, 0xcc},
	  {0x7a, 0xbb, 0xa8, 0xdb, 0x4b, 0xff, 0xe7, 0xf8},
	  {0x7a, 0xbb, 0xaa, 0xdb, 0x6a, 0xff, 0x37, 0x78},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1c, 0x18},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1e, 0x38},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1e, 0x1c},
	  {0x7f, 0xff, 0xff, 0xea, 0x8e, 0xa3, 0x0f, 0x7a},
	  {0x7f, 0xff, 0xff, 0xea, 0xae, 0xaf, 0x0f, 0xf8},
	  {0x7f, 0xff, 0xff, 0xea, 0xae, 0xa3, 0x03, 0xf8},
	  {0x7f, 0xff, 0xff, 0xe2, 0x2e, 0x2f, 0x11, 0xd0},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0x10},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20},
	  {0x7f, 0xf6, 0x5d, 0x5f, 0xf7, 0xf6, 0xbf, 0xe0},
	  {0x7f, 0xe0, 0x08, 0x07, 0x80, 0xe0, 0x1f, 0x80},
	  {0x7f, 0xf2, 0xd8, 0x43, 0x94, 0xe1, 0x7f, 0x00},
	  {0x7f, 0xf3, 0xf8, 0xc1, 0x18, 0x41, 0x7f, 0x00},
	  {0x7f, 0xf0, 0xd8, 0x70, 0x1c, 0x07, 0xff, 0xe8},
	  {0x7f, 0xe0, 0x08, 0x70, 0x00, 0x07, 0xff, 0xf8},
	  {0x7f, 0xfb, 0x98, 0x70, 0x00, 0x47, 0xff, 0xe0},
	  {0x7f, 0xf3, 0x18, 0xc1, 0x0c, 0xc1, 0x7f, 0xe0},
	  {0x7f, 0xe3, 0x18, 0x03, 0x8c, 0x60, 0x7f, 0x80},
	  {0x7f, 0xe0, 0x18, 0x07, 0xc0, 0x60, 0x1f, 0xd0},
	  {0x7f, 0xe7, 0xb8, 0xf7, 0xde, 0xf7, 0xbf, 0xf8},
	  {0x7f, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xf8},
	  {0x7f, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0x70},
	  {0x7f, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0x00},
	  {0x7f, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xc0},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0},
	  {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x80},
	  {0x3f, 0xff, 0xff, 0xff, 0xee, 0xff, 0xfc, 0x80},
	  {0x00, 0x00, 0x00, 0x00, 0x1e, 0x7f, 0xf8, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0xfc, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x14, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},

	 {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	  {0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00},
	  {0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00},
	  {0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00},
	  {0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00},
	  {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00},
	  {0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00},
	  {0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x83, 0x80},
	  {0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0x80},
	  {0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe6, 0xc0},
	  {0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0x80},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0x00},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00},
	  {0x38, 0x88, 0x88, 0x81, 0x68, 0xff, 0xf0, 0x40},
	  {0x3a, 0xab, 0xaa, 0xdb, 0x2b, 0xff, 0xe4, 0xc0},
	  {0x3a, 0x88, 0x8a, 0xdb, 0x0a, 0xff, 0xff, 0xe0},
	  {0x3a, 0xbb, 0x98, 0xdb, 0x4a, 0xff, 0xff, 0xfc},
	  {0x38, 0xb8, 0xaa, 0xd1, 0x68, 0xff, 0x8f, 0xfc},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbe, 0x38},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbc, 0x1c},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8c, 0x3e},
	  {0x3f, 0xff, 0xff, 0xe2, 0x86, 0x23, 0x8e, 0x3c},
	  {0x3f, 0xff, 0xff, 0xea, 0xae, 0xaf, 0x9f, 0xf0},
	  {0x3f, 0xff, 0xff, 0xea, 0xae, 0xa3, 0x9f, 0xf0},
	  {0x3f, 0xff, 0xff, 0xe2, 0x2e, 0x2f, 0x8f, 0xb8},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9d, 0x80},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xc0},
	  {0x3f, 0xf8, 0x38, 0x87, 0xc1, 0xf8, 0x7f, 0xc0},
	  {0x3f, 0xf0, 0x18, 0x03, 0x80, 0xf0, 0x3f, 0xc0},
	  {0x3f, 0xe1, 0x38, 0x03, 0x08, 0x60, 0x3f, 0x80},
	  {0x3f, 0xe1, 0xf8, 0x63, 0x1c, 0x63, 0xff, 0x80},
	  {0x3f, 0xe0, 0x38, 0xe3, 0x00, 0x63, 0xff, 0xf0},
	  {0x3f, 0xf0, 0x18, 0xe3, 0x00, 0x63, 0xff, 0xf0},
	  {0x3f, 0xfc, 0x08, 0xe3, 0x1f, 0xe3, 0xff, 0xf0},
	  {0x3f, 0xff, 0x88, 0x63, 0x1f, 0xe3, 0xff, 0xc0},
	  {0x3f, 0xf0, 0x08, 0x03, 0x00, 0xe0, 0x3f, 0xc0},
	  {0x3f, 0xe0, 0x18, 0x03, 0x80, 0xf0, 0x3f, 0xe0},
	  {0x3f, 0xf8, 0x78, 0x0f, 0xe1, 0xf8, 0x7f, 0xf0},
	  {0x3f, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xf0},
	  {0x3f, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x80},
	  {0x3f, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x80},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0},
	  {0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0},
	  {0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x0c, 0xff, 0xfc, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0xe3, 0x88, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x80, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}};


static uint8_t logo_OutOfSpec_getChunk(unsigned char const nBitPlane,
                                       unsigned char const nChunkX,
                                       unsigned char const nChunkY,
                                       unsigned int const nFrame)
{
	assert(nBitPlane < 2);
	assert(nChunkX < 8);
	assert(nChunkY < 55);

	return pgm_read_byte(&aOutOfSpecBitmap[nBitPlane][nChunkY][nChunkX]);
}


void logo_OutOfSpec()
{
	// width 64, height 55, 2 bitplanes (4 colors), 600 frames à 50ms (20 fps)
	bitmap_scroll(64, 55, 2, 600, 50, logo_OutOfSpec_getChunk);
}
