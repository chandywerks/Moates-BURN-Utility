/*
chipdata

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Function returns chip data defining EEPROMS supported by the BURNII such
as chip size and command headers.
*/

#include "chipdata.h"

Chip SST27SF512 = (Chip) {
	0,
	0xFFFF,
	2,
	"5R",
	"5W",
	"5E"
};

Chip AT29C256 = (Chip) {
	0,
	0x7FFF,
	2,
	"2R",
	"2W",
	"2E"
};

Chip AM29F040 = (Chip) {
	0,
	0x7FFFF,
	3,
	"4R",
	"4W",
	"4E"
};

Chip _2732A = (Chip) {
	0,
	0xFFF,
	2,
	"3R",
	"\0",
	"\0"
};

Chip *chip_select(char *str) {
	if (!strcmp(str, "SST27SF512"))
		return &SST27SF512;
	else if (!strcmp(str, "AT29C256"))
		return &AT29C256;
	else if (!strcmp(str, "AM29F040"))
		return &AM29F040;
	else if (!strcmp(str, "2732A"))
		return &_2732A;
	else	
		return NULL;
}
