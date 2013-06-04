/*
promio

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Functions for erasing, reading, and writing to eeproms
supported by the BURNII.
*/

#include "promio.h"

int read_prom(int fd, int chip_id, char *file)
{
	return 0;
}
int write_prom(int fd, int chip_id, char *file)
{
	// TODO at the moment this only works for burning an SST 27SF512
	// we need to put chip data, like size and header command / length, 
	// in a data structure and refrence that instead so this process will work
	// for any given chip.

	int chip_size;
	int address=0;	// Starting address
	int offset;		// cmd data offset
	int i;			// bytes to read and general purpose counter
	int n;			// n bytes to write
	char *buf;		// Buffer for reading file in
	char *cmd;		// Command string
	char *adr;		// Address string
	char *ret;		// Pointer to return string
	
	FILE *fp;
	fp=fopen(file,"r");
	if(fp==NULL)
		die("Unable to read from file: %s\n",file);
	buf=malloc(sizeof(char)*256);

	// Erase chip
	if(erase_prom(fd,chip_id)>0)
		return 1;
	offset=5;
	chip_size=0xFFFF;

	while(address<=chip_size)
	{
		i=(chip_size-address>=256)?256:chip_size-address;	// n bytes to read
		// Read at most 256 bytes or until EOF from file
		if((n=fread(buf,sizeof(char),i,fp))==0)
			break;

		cmd=malloc(sizeof(char)*(offset+n));
		adr=addrstr(address,2);			// 2 bytes for address

		for(i=offset;i<n+offset;i++)	// Write data string from buffer
			cmd[i]=buf[i-offset];
		// Generate command string
		cmd[0]='5';
		cmd[1]='W';
		cmd[2]=(n==256)?0:n;		// n bytes to write (0 for 256)
		cmd[3]=adr[0];				// MSB
		cmd[4]=adr[1];				// LSB

		if(*send(fd,cmd,offset+n,1)!='O')	// write data to EEPROM
			return 1;				
		address+=n;					// increment address
	}
	fclose(fp);
	return 0;
}
int erase_prom(int fd, int chip_id)
{
	char *cmd;
	char *ret;
	int i;

	switch(chip_id)
	{
		case 0:		// SST27SF512
			cmd=malloc(sizeof(char)*2);
			ret=malloc(sizeof(char)*1);
			cmd = "\x35\x45";
			return (*send(fd,cmd,2,1))=='O'?0:1;
		case 1:		// AT29C256
			return 0;
		case 2:		// AM29F040
			return 0;
		case 3:		// 2732A
			printf("Read only support for the 2732A.\n");
			return 1;
		default:
			return 1;
	}
}
char * addrstr(int address, int n)
{
	char *str=malloc(sizeof(char)*n);
	int i;
	for(i=0;i<n;i++)
	{
		str[n-i-1]=address&0xFF;
		address>>=8;
	}	
	return str;
}
