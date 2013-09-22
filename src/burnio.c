/*
burnio

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Functions for configuring the emulated serial port for
the BURNII and sending/reciving commands.
*/

#include "burnio.h"

char *send(int fd, char *cmd, int n_write, int n_read)
{
	// Sends a command and returns response data
	char *response=malloc(sizeof(char)*n_read);
	char new_cmd[n_write+1];
	char sum=0;
	int i;
	
	for(i=0;i<n_write;i++)
		sum+=new_cmd[i]=cmd[i];	// Calculate checksum & copy input data to new buffer
	new_cmd[n_write++]=sum;		// Tack on checksum

	if(write(fd,new_cmd,n_write) != n_write)		// Send command
		die("Error writing to the device.");

	i=0;
	while(i<n_read)									// Read response
		if((i+=read(fd,response+i,n_read-i))<=0)
			die("Error writing to the device.");

	return response;
}
int config(char *device)
{
	int fd;
	// Open serial device /dev/ttyUSB0 with R+W, no control over the terminal, and non-blocking I/O
	if((fd = open(device, O_RDWR | O_NOCTTY))<0)
		die("Unable to open device %s. Perhaps you are not a member of the dialout group?\n",device);
	// Configure communication params and interface properties
	// We need 921600 baud 8n1 and non-canonical mode
	// See "termios" man page for details
	struct termios settings;
	speed_t speed;

	if(tcgetattr(fd, &settings)<0)
		die("Unable to get attibutes for %s. Is this a serial device?\n",device);
	
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
		die("Unable to configure %s",device);

	return fd;
}
