/* Automatically generated by mkrunetype.awk */
#include "runetype.h"
#define ISRUNE isspacerune
#define SINGLES singles
static Rune singles[][1] = {
	{ 0x0085 },
	{ 0x00A0 },
	{ 0x1680 },
	{ 0x202F },
	{ 0x205F },
	{ 0x3000 },
};
#define RANGES ranges
static Rune ranges[][2] = {
	{ 0x0009, 0x000D },
	{ 0x001C, 0x0020 },
	{ 0x2000, 0x200A },
	{ 0x2028, 0x2029 },
};
#include "runetypebody.h"