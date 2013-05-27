#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>

// Linux drivers for the Moates BURN I and II EEPROM burner.
// Chris Handwerker - chris.handwerker@gmail.com
// www.homebrewtechnology.org

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
char * send(int fd, char *cmd, int n_write, int n_read)
{
	// Sends a command and returns response data
	char *response=malloc(sizeof(char)*n_read);
	char new_cmd[n_write+1];
	char sum=0;
	int i;
	
	for(i=0;i<n_write;i++)
	{
		new_cmd[i]=cmd[i];
		sum+=cmd[i];
	}
	new_cmd[n_write++]=sum;

	if(write(fd,new_cmd,n_write) != n_write)		// Send command
		die("Error writing to the device.");
	if(read(fd,response,n_read) != n_read)
		die("Error reading response from device");
	return response;
}

int read_prom(int fd, int chip_id, char *file)
{
	return 0;
}
int write_prom(int fd, int chip_id, char *file)
{
	return 0;
}
int erase_prom(int fd, int chip_id)
{
	char *cmd;
	char *ret;

	switch(chip_id)
	{
		case 0:
			cmd=malloc(sizeof(char)*2);
			ret=malloc(sizeof(char)*1);
			cmd = "\x35\x45";
			ret = send(fd, cmd, 2, 1);
			if(ret[0]=='O')
				return 0;
			else
				return 1;
		case 1:
			return 0;
		case 2:
			return 0;
		case 3:
			return 0;
	}
}
int config(char *device)
{
	int fd;
	// Open serial device /dev/ttyUSB0 with R+W, no control over the terminal, and non-blocking I/O
	if((fd = open(device, O_RDWR | O_NOCTTY))<0)
	{
		fprintf(stderr,"Unable to open device %s. Perhaps you are not a member of the dialout group?\n",device);
		return -1;
	}
	// Configure communication params and interface properties
	// We need 921600 baud 8n1 and non-canonical mode
	// See "termios" man page for details
	struct termios settings;
	speed_t speed;

	if(tcgetattr(fd, &settings)<0)
	{
		fprintf(stderr,"Unable to get attibutes for %s. Is this a serial device?\n",device);
		return -1;
	}	
	
	// Input settings
	settings.c_iflag = IGNPAR;	// Ignore framing & parity errors	
	// Output settings
	settings.c_oflag = 0;
	// Control modes
	settings.c_cflag = CS8 | CREAD | CBAUDEX;	// Set char size (8) & enable receiver & allow speeds above 57600 baud
	// Local modes
	settings.c_lflag = 0;
	// Set up non-canonical input processing
	settings.c_cc[VTIME] = 0;	// No timeout, waits forever until n bytes received
	settings.c_cc[VMIN] = 1;	// Number of bytes needed to return from read()
	// Set input and output baud rate to 921600 baud
	cfsetispeed(&settings, B921600);
	cfsetospeed(&settings, B921600);
	// Apply settings
	if(tcsetattr(fd, TCSAFLUSH, &settings)<0)	// Flush data currently on port buffer and apply
	{
		fprintf(stderr,"Unable to configure %s",device);
		return -1;
	}
	return fd;
}
int main(int argc, char *argv[])
{
	p_name=argv[0];
	int fd;		// File descriptor
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
	while((c = getopt(argc,argv,"d:c:r:w:e")) != -1)
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
		if(read_prom(fd,chip_id,read_file)>0)
			die("Error reading from device.");
		else
			printf("Sucessfully read from the chip!\n");
	}
	else if(wflag)
	{
		if(write_prom(fd,chip_id,write_file)>0)
			die("Error writing to device.");
		else
			printf("Sucessfully wrote to the chip!\n");
	}
	else if(eflag)
	{
		if(erase_prom(fd,chip_id)>0)
			die("Error erasing device."); 
		else
			printf("Sucessfully erased the chip!\n");
	}
	else
		help("No action specified.\n");
	return 0;
}
