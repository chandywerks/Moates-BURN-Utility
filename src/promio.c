/*
promio

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Functions for erasing, reading, and writing to eeproms
supported by the BURNII.
*/

#include "promio.h"

int read_prom(int fd, Chip *chip, char *file)
{
	int address=chip->offset;	// Starting address
	int offset;					// cmd data offset
	int i;						// Counter
	int n;						// n bytes to read from chip
	char *cmd;					// Command string
	char *adr;

	FILE *fp;
	fp=fopen(file,"w");
	if(fp==NULL)
		die("Unable to write to file: %s\n",file);

	offset=3+chip->naddr;		// Command header + n bytes of address	

	// Generate command string
	cmd=malloc(sizeof(char)*offset);
	cmd[0]=chip->rcmd[0];
	cmd[1]=chip->rcmd[1];

	while(address<chip->size)
	{
		n=(chip->size-address>=256)?256:chip->size-address;	// n bytes to read from chip
		
		// Generate command string to read n bytes from chip
		adr=addrstr(address,chip->naddr);
		cmd[2]=(n==256)?0:n;
		for(i=0;i<chip->naddr;i++)
			cmd[i+3]=adr[i];	

		fwrite(send(fd,cmd,offset,n),sizeof(char),n,fp); // Write data to file

		address+=n;	// Increment address
	}
	free(cmd);

	return 0;
}
int write_prom(int fd, Chip *chip, char *file)
{
	int address=chip->offset;	// Starting address
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
	offset=3+chip->naddr;	// Command header + n bytes for address

	if(erase_prom(fd, chip)>0)	// Erase the chip before writing
		return 1;	

	while(address<=chip->size)
	{
		i=(chip->size-address>=256)?256:chip->size-address;	// n bytes to read from file
		// Read at most 256 bytes or until EOF from file
		if((n=fread(buf,sizeof(char),i,fp))==0)
			break;

		cmd=malloc(sizeof(char)*(offset+n));
		adr=addrstr(address,chip->naddr);	 // n bytes for address

		for(i=offset;i<n+offset;i++)	// Write data string from buffer
			cmd[i]=buf[i-offset];

		// Generate command string
		cmd[0]=chip->wcmd[0];
		cmd[1]=chip->wcmd[1];
		cmd[2]=(n==256)?0:n;		// n bytes to write (0 for 256)
		for(i=0;i<chip->naddr;i++)
			cmd[i+3]=adr[i];			// specify start address

		if(*send(fd,cmd,offset+n,1)!='O')	// write data to EEPROM
			return 1;				

		free(cmd);
		address+=n;					// increment address
	}
	fclose(fp);
	return 0;
}
int erase_prom(int fd, Chip *chip)
{
	char *cmd;
	char *ret;

	if(chip->ecmd[0]=='\0')
		die("Chip specificed is read only.");

	if(chip->ecmd[0]=='4')		// Special case for AM29F040
	{
		// TODO implement AM29F040 erase
		printf("Can't erase an AM29F040 quite yet...\n");
		return 1;
	}
	else
		return (*send(fd,chip->ecmd,2,1))=='O'?0:1;
}
char *addrstr(int address, int n)
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
