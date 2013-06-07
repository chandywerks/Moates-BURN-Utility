/*
burn2

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Linux drivers for the Moates BURN II EEPROM burner. 
*/


#include <stdio.h>
#include <stdarg.h>

#include "promio.h"

const char *p_name;

void help(char *errmsg, ...)
{
	va_list va;
	va_start(va, errmsg);
	vprintf(errmsg, va);
	va_end(va);
	printf("\n\nDriver for the Moates BURN EEPROM burner.\nArguments\n\t-d FILE\n\t\tSpecify a device file, defaults to /dev/ttyUSB0\n\n\t-c WORD\n\t\tSpecify the chip type, supported ships are...\n\t\t\t- SST27SF512\n\t\t\t- AT29C256\n\t\t\t- AM29F040\n\t\t\t- 2732A \t(Read-Only)\n\n\t-r FILE\n\t\tSpecify a file to dump EEPROM data to\n\n\t-w FILE\n\t\tSpecify a file to write to the EEPROM\n\n\t-e\n\t\tErase contents of the EEPROM\n");
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
	int chip_id;
	int rflag = 0;
	int wflag = 0;
	int eflag = 0;
	char *chip = NULL;
	char *device_file = NULL;
	char *read_file = NULL;
	char *write_file = NULL;

	// Get args
	while((c = getopt(argc, argv, "d:c:r:w:e")) != -1)
	{
		switch(c)
		{
			case 'd':
				device_file = optarg;
				break;
			case 'c':
				chip = optarg;
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
				return 1;
		}
	}
	if(device_file == NULL)
		device_file = "/dev/ttyUSB0";
	if(chip == NULL)
		help("You must specify a chip type.");

	if(!strcmp(chip,"SST27SF512"))
		chip_id=0;
	else if(!strcmp(chip,"AT29C256"))
		chip_id=1;
	else if(!strcmp(chip,"AM29F040"))
		chip_id=2;
	else if(!strcmp(chip,"2732A"))
		chip_id=3;
	else	
		help("Invalid chip type: %s",chip);

	// Configure the serial port and perform requested action
	if((fd=config(device_file))==-1) return 1;

	if(rflag)
	{
		if(read_prom(fd, chip_id, read_file)>0)
			die("Error reading from device.");
		else
			printf("Sucessfully read from the chip!\n");
	}
	else if(wflag)
	{
		if(write_prom(fd, chip_id, write_file)>0)
			die("Error writing to device.");
		else
			printf("Sucessfully wrote to the chip!\n");
	}
	else if(eflag)
	{
		if(erase_prom(fd, chip_id)>0)
			die("Error erasing device.");
		else
			printf("Sucessfully erased the chip!\n");
	}
	else
		help("No action specified.\n");
	return 0;
}
