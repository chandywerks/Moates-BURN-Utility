/*
burnio

Chris Handwerker (2013) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Functions for configuring the emulated serial port for
the BURNII and sending/reciving commands.
*/

#ifndef BURNIO_H
#define BURNIO_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

extern char * send(int fd, char *cmd, int n_write, int n_read);
extern int config(char *device);

#endif
