#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// Linux drivers for the Moates BURN I and II EEPROM burner.
// Chris Handwerker - chris.handwerker@gmail.com
// www.homebrewtechnology.org


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
int read_prom(int fd, char *chip, char *file)
{
	return 0;
}
int write_prom(int fd, char *chip, char *file)
{
	return 0;
}
int erase_prom(int fd, char *chip)
{
	return 0;
}
void help()
{
	printf("\nDriver for the Moates BURN EEPROM burner.\n\nArguments\n\t-d FILE\n\t\tSpecify a device file, defaults to /dev/ttyUSB0\n\n\t-c WORD\n\t\tSpecify the chip type, supported ships are...\n\t\t\t- SST27SF512\n\t\t\t- AT29C256\n\t\t\t- AM29F040\n\t\t\t- 2732A \t(Read-Only)\n\n\t-r FILE\n\t\tSpecify a file to dump EEPROM data to\n\n\t-w FILE\n\t\tSpecify a file to write to the EEPROM\n\n\t-e\n\t\tErase contents of the EEPROM\n");
}
int main(int argc, char *argv[])
{
	int fd;		// File descriptor
	int c;
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
	{
		printf("You must specify a chip type.\n");
		help();
		return 1;	
	}
	if(!(!strcmp(chip,"SST27SF512")||!strcmp(chip,"AT29C256")||!strcmp(chip,"AM29F040")||!strcmp(chip,"2732A")))
	{
		printf("Invalid chip type specified.\n");
		help();
		return 1;
	}

	// Configure the serial port and perform requested action
	if((fd=config(device_file))==-1) return 1;

	if(rflag)
		return read_prom(fd,chip,read_file);
	else if(wflag)
		return write_prom(fd,chip,write_file);
	else if(eflag)
		return erase_prom(fd,chip);
	else
	{
		printf("No action specified.\n");
		help();
		return 1;
	}
}
/*
	// Testing it out...
	// Sending 'VV' to the device to request its version number
	// It should respond with 3 bytes of data
	char *cmd = "\x56\x56";
	int n=2;	// num bytes to send
	int r=3;	// num bytes we get back

	int i;
	char buffer[r];

	int n_written;
	int n_read;

	n_written = write(fd,cmd,n);
	printf("We wrote %d bytes\n",n_written);

	if((n_read=read(fd,buffer,r))<0)
	{
		printf("Error reading from the device\n");
		return 1;
	}
	
	printf("We read %d bytes back\n\n",n_read);

	for(i=0;i<r;i++)
		printf("0x%hhx ",buffer[i]);

	printf("\n");
	close(fd);

	return 0;
*/
