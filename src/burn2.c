/*
burn2

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Linux drivers for the Moates BURN II EEPROM burner. 
*/


#include <stdio.h>
#include <stdarg.h>

#include "promio.h"
#include "chipdata.h"

const char *p_name;

void help(char *errmsg, ...)
{
	va_list va;
	va_start(va, errmsg);
	vprintf(errmsg, va);
	va_end(va);
	printf
	(
		"\n\nDriver for the Moates BURN II EEPROM burner.\n"
		"Arguments\n"
		"\t-d FILE\n\t\tSpecify a device file, defaults to /dev/ttyUSB0\n\n"
		"\t-c WORD\n\t\tSpecify the chip type, supported ships are...\n"
		"\t\t\t- SST27SF512\n"
		"\t\t\t- AT29C256\n"
		"\t\t\t- AM29F040\n"
		"\t\t\t- 2732A \t(Read-Only)\n\n"
		"\t-r FILE\n"
		"\t\tSpecify a file to dump EEPROM data to\n\n"
		"\t-w FILE\n"
		"\t\tSpecify a file to write to the EEPROM\n\n"
		"\t-e\n\t\t"
		"Erase contents of the EEPROM\n"
		"\t-a\n\t\t"
		"Address range (defaut is 0 to max chip size) to read or write at\n\t\t"
		"Format: 0x0-0xFFFF\n\n"
		"\t-o\n\t\t"
		"Address offset (default is 0) to start reading or writing at\n\t\t"
		"Format: 0xFF\n\n"
	);
	exit(0);
}
void die(char *errmsg, ...)
{
	fprintf(stderr, "%s: ", p_name);
	va_list va;
	va_start(va, errmsg);
	vfprintf(stderr, errmsg, va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(1);
}
int main(int argc, char *argv[])
{
	p_name=argv[0];

	int fd;	// File descriptor
	int c;
	int rflag, wflag, eflag, aflag, oflag;	// Argument flags
	int addr1, addr2;		// address range

	char *chipstr = NULL;
	char *device_file = NULL;
	char *read_file = NULL;
	char *write_file = NULL;

	Chip *chip = NULL;		// chipdata struct

	rflag=wflag=eflag=aflag=oflag=0;
	// Get args
	while((c = getopt(argc, argv, "a:o:d:c:r:w:e")) != -1)
	{
		switch(c)
		{
			case 'a':
				sscanf(optarg, "%x-%x", &addr1, &addr2);
				aflag = 1;
				break;
			case 'o':
				sscanf(optarg, "%x", &addr1);
				oflag = 1;
				break;
			case 'd':
				device_file = optarg;
				break;
			case 'c':
				chipstr = optarg;
				break;
			case 'r':
				read_file = optarg;
				rflag = 1;
				break;
			case 'w':
				write_file = optarg;
				wflag = 1;
				break;
			case 'e':
				eflag = 1;
				break;
			case '?':
				help("");
				return 1;
		}
	}
	if(device_file == NULL)
		device_file = "/dev/ttyUSB0";
	if(chipstr == NULL)
		help("You must specify a chip type.");
	if((chip=chip_select(chipstr))==NULL)
		die("Invalid chip type '%s'",chipstr);

	// Validate and set sepcified address range or offset or default to entire chip with offset 0
	if(aflag)
	{
		if((addr1>addr2) || (addr1<0) || (addr2>chip->size))
			help("Invalid address range specified.");
		chip->offset=addr1;
		chip->size=addr2;
	}
	else if(oflag)
	{
		if(addr1<0 || addr1>chip->size)
			help("Invalid offset specified. Must be a value between 0x0 and 0x%X", chip->size);
		chip->offset=addr1;
	}

	// Configure the serial port and perform requested action
	if((fd=config(device_file))==-1)
		return 1;

	if(rflag)
	{
		if(read_prom(fd, chip, read_file)>0)
			die("Error reading from device.");
		else
			printf("Sucessfully read from the chip!\n");
	}
	else if(wflag)
	{
		if(write_prom(fd, chip, write_file)>0)
			die("Error writing to device.");
		else
			printf("Sucessfully wrote to the chip!\n");
	}
	else if(eflag)
	{
		if(erase_prom(fd, chip)>0)
			die("Error erasing device.");
		else
			printf("Sucesfully erased the chip!\n");
	}
	else
		help("No action specified.\n");

	return 0;
}
