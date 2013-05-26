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
	if((fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
	{
		fprintf(stderr,"Unable to open device %s. Perhaps you are not a member of the dialout group?\n",device);
		return -1;
	}
	// Configure communication params and interface properties
	// We need 921600 baud 8n1 and non-canonical mode
	// See "termios" man page for details
	struct termios settings;
	speed_t speed;

	if(tcgetattr(fd, &settings)==-1)
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
	if(tcsetattr(fd, TCSAFLUSH, &settings)==-1)	// Flush data currently on port buffer and apply
	{
		fprintf(stderr,"Unable to configure %s",device);
		return -1;
	}
	return fd;
}

int main(int argc, char *argv[])
{
	char *device = "/dev/ttyUSB0";
	int fd;
	if((fd=config(device))==-1) return 1;

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

	while((n_read=read(fd,buffer,r))>0){}
	printf("We read %d bytes back\n\n",n_read);

	for(i=0;i<r;i++)
		printf("0x%hhx ",buffer[i]);

	printf("\n");
	close(fd);

	return 0;	
}
