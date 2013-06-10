/*
chipdata

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Function returns chip data defining EEPROMS supported by the BURNII such
as chip size and command headers.
*/

#ifndef CHIPDATA_H
#define CHIPDATA_H

#include <stdlib.h>
#include <string.h>

typedef struct
{
	int size;
	int naddr;		// Bytes of address
	char rcmd[2];	// Read command header
	char wcmd[2];	// Write command header
	char ecmd[2];	// Erase command header
} Chip;

extern Chip * chip_select(char *chip);

#endif
