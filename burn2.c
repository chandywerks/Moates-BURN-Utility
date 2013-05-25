#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd;	
	char *device = "/dev/ttyUSB0";

	// Open serial device /dev/ttyUSB0 with R+W, no control over the terminal, and non-blocking I/O
	if((fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
	{
		fprintf(stderr,"Unable to open device %s. Perhaps you are not a member of the dialout group?\n",device);
		return 1;
	}

	// Configure communication params and interface properties
	// We need 921600 baud 8n1 and non-canonical mode
	// See "termios" man page for details
	struct termios settings;
	speed_t speed;

	if(tcgetattr(fd, &settings)==-1)
	{
		fprintf(stderr,"Unable to get attibutes for %s. Is this a serial device?\n",device);
		return 1;
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
	if(tcsetattr(fd, TCSAFLUSH, &settings)==-1)	// Flush data currently on port buffer and apply
	{
		fprintf(stderr,"Unable to configure %s",device);
		return 1;
	}

	// We can use standard unix system calls read() and write()
	
	char *cmd = "\x35\x57\x01\x00\x00\x78\x05";
	//char *cmd = "\x35\x45\x7a";
	char buffer[BUFSIZ];

	int n=7;

	int n_written;
	int n_read;

	n_written = write(fd,cmd,n);
	printf("We wrote %d bytes\n",n_written);

	//TODO writing is sucessful but reading back the response
	// returns -1. Get this fixed and we are in bussiness :)	
	n_read = read(fd,buffer,1);
	printf("We read back %d bytes: %c\n",n_read,buffer[0]);

	close(fd);

	return 0;	
}
