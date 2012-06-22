/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <curses.h>
#include <sys/ioctl.h>

static int fd;

/* rts's logic is inverted, since rs232 defines it as active low */
void serial_set_rts(int en)
{
	int status;

	ioctl(fd, TIOCMGET, &status);
	if (en)
		status &= ~TIOCM_RTS;
	else
		status |= TIOCM_RTS;
	ioctl(fd, TIOCMSET, &status);
}

int serial_get_rts(void)
{
	int status;
	ioctl(fd, TIOCMGET, &status);
	return !!(status & TIOCM_RTS);
}

void serial_set_dtr(int en)
{
	int status;

	ioctl(fd, TIOCMGET, &status);
	if (en)
		status |= TIOCM_DTR;
	else
		status &= ~TIOCM_DTR;
	ioctl(fd, TIOCMSET, &status);
}

int serial_get_dtr(void)
{
	int status;
	ioctl(fd, TIOCMGET, &status);
	return !!(status & TIOCM_DTR);
}

int serial_open(const char *port)
{
    int i, serial, status;

    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		fprintf(stderr, "error opening serial\n");
		return -1;
	}

	/* set the serial port */
	struct termios s;
	tcgetattr(fd, &s);
	s.c_cflag = CS8 | CREAD;
	s.c_ispeed = s.c_ospeed = 115200;
	tcsetattr(fd, TCSANOW, &s);

	return 0;
}

void serial_close(void)
{
	close(fd);
}

int serial_get_fd(void)
{
	return fd;
}


