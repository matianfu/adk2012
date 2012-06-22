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
#include <fcntl.h>

#include "serial.h"

/* RTS is erase, asserted high */
static void set_erase(int en)
{
	serial_set_rts(en);
}

static int get_erase(void)
{
	return serial_get_rts();
}

/* DTR is reset, asserted low */
static void set_reset(int en)
{
	serial_set_dtr(!en);
}

static int get_reset(void)
{
	return !serial_get_dtr();
}

static void secret_knock(void)
{
    int i;
    const int magic = 0xac5a;

    set_reset(1);
    set_erase(0);

    for (i = 0; i < 16; i++) {
        set_erase(!(~(magic >> i) & 1));
        set_reset(0);
        usleep(1000);
        set_reset(1);
    }
    set_erase(0);
    set_reset(0);
}

#ifndef __WIN32
#include <termios.h>
#include <poll.h>

/* raw terminal stuff */
static struct termios oldstdin;
static struct termios oldstdout;


static void resetconsole(void)
{
	tcsetattr(0, TCSANOW, &oldstdin);
	tcsetattr(1, TCSANOW, &oldstdout);
}

static void setconsole(void)
{
	struct termios t;

	tcgetattr(0, &oldstdin);
	tcgetattr(1, &oldstdout);

	atexit(&resetconsole);

	t = oldstdin;
	t.c_lflag = ISIG; // no input processing
	// Don't interpret various control characters, pass them through instead
	t.c_cc[VINTR] = t.c_cc[VQUIT] = t.c_cc[VSUSP] = '\0';
	t.c_cc[VMIN]  = 0; // nonblocking read
	t.c_cc[VTIME] = 0; // nonblocking read
	tcsetattr(0, TCSANOW, &t);

	fcntl(0, F_SETFL, O_NONBLOCK);

	t = oldstdout;
	t.c_lflag = ISIG; // no output processing
	tcsetattr(1, TCSANOW, &t);
}

static void console(int serial_fd)
{
	struct pollfd pfd[2];
	char c;
	int err;

	printf("starting console mode, ctrl-c to exit\n");
	fflush(stdout);
	setconsole();

	pfd[0].fd = STDIN_FILENO;
	pfd[0].events = POLLIN | POLLERR;
	pfd[1].fd = serial_fd;
	pfd[1].events = POLLIN | POLLERR;

	for (;;) {
		err = poll(pfd, 2, -1);
		if (err <= 0)
			break;

		if (pfd[0].revents & POLLIN) {
			err = read(STDIN_FILENO, &c, 1);
			if (err < 0)
				break;

			write(serial_fd, &c, 1);
		}
		if (pfd[1].revents & POLLIN) {
			err = read(serial_fd, &c, 1);
			if (err < 0)
				break;
			write(STDOUT_FILENO, &c, 1);
		}

		if ((pfd[0].revents | pfd[1].revents) & POLLERR)
			break;
	}
}
#endif

static void usage(int argc, char **argv)
{
	fprintf(stderr, "usage: %s <serial port> [commands ...]\n", argv[0]);
	fprintf(stderr, "valid commands are:\n");
	fprintf(stderr, "\terase: perform a handshake with a microcontroller to erase the flash\n");
	fprintf(stderr, "\treset: toggle the reset line\n");
	fprintf(stderr, "\tolderase: toggle the erase line\n");
#ifndef __WIN32
	fprintf(stderr, "\tconsole: go into console mode on the serial line\n");
	fprintf(stderr, "\tmonitor: display the current status of the lines\n");
	fprintf(stderr, "\thold: keep the uart open and wait for ctrl-c\n");
#endif

	exit(1);
}

int main(int argc, char **argv)
{
    int i, serial, status;

	if (argc < 3) {
		fprintf(stderr, "not enough arguments\n");
		usage(argc, argv);
	}

	const char *port;
	port = argv[1];

	if (serial_open(port) < 0) {
		fprintf(stderr, "error opening serial\n");
		return 1;
	}

	int arg;
	for (arg = 2; arg < argc; arg++) {
		if (!strcasecmp(argv[arg], "erase")) {
			printf("erasing...");
			set_reset(1);
			usleep(500000);
			set_reset(0);
			secret_knock();
			usleep(1000000);
			printf("done.\n");
		} else if (!strcasecmp(argv[arg], "olderase")) {
			set_reset(0);
			set_erase(1);
			usleep(300000);

			set_erase(0);
			set_reset(1);
			usleep(100000);
			set_reset(0);
		} else if (!strcasecmp(argv[arg], "reset")) {
			printf("resetting...");
			set_erase(0);
			set_reset(0);
			usleep(100000);
			set_reset(1);
			usleep(500000);
			set_reset(0);
			printf("done.\n");
#ifndef __WIN32
		} else if (!strcasecmp(argv[arg], "monitor")) {
			printf("monitoring state of erase and reset lines\n");
			int r, e;

			e = get_erase();
			r = get_reset();

			for (;;) {
				int e1 = get_erase();
				if (e1 != e)
					printf("erase %d\n", e1);

				int r1 = get_reset();
				if (r1 != r)
					printf("reset %d\n", r1);

				e = e1;
				r = r1;
			}
		} else if (!strcasecmp(argv[arg], "hold")) {
			printf("holding port open, ctrl-c to exit.\n");
			set_erase(0);
			set_reset(0);
			for (;;)
				usleep(1000000);
		} else if (!strcasecmp(argv[arg], "console")) {
			console(serial_get_fd());
#endif
		} else {
			printf("bad command '%s'\n", argv[arg]);
			exit(1);
		}
	}

	serial_close();
}

