/*
promio

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Functions for erasing, reading, and writing to eeproms
supported by the BURNII.
*/

#ifndef PROMIO_H
#define PROMIO_H

#include <stdio.h>
#include <stdlib.h>

#include "burnio.h"

extern int read_prom(int fd, int chip_id, char *file);
extern int write_prom(int fd, int chip_id, char *file);
extern int erase_prom(int fd, int chip_id);

static char * addrstr(int address, int n);

#endif
