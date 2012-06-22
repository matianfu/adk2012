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
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static HANDLE serial;

int serial_get_dtr(void)
{
	return 0;
}

int serial_get_rts(void)
{
	return 0;
}

void serial_set_dtr(int val)
{
	if (val)
		EscapeCommFunction(serial, SETDTR);
	else
		EscapeCommFunction(serial, CLRDTR);
}

/* rts's logic is inverted, since rs232 defines it as active low */
void serial_set_rts(int val)
{
	if (val)
		EscapeCommFunction(serial, CLRRTS);
	else
		EscapeCommFunction(serial, SETRTS);
}

int serial_open(const char *port)
{
	// open the port
	serial = CreateFile(port,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	//printf("serial handle %u\n", serial);

	if (serial == INVALID_HANDLE_VALUE)
		return -1;

	//SetupComm(serial, 1024, 1024);

	// config it
	DCB config;
	GetCommState(serial, &config);
	config.BaudRate = 115200;
	config.ByteSize = 8;
	config.Parity = NOPARITY;
	config.StopBits = ONESTOPBIT;
	config.fBinary = TRUE;
	config.fParity = TRUE;
	config.fDtrControl = DTR_CONTROL_ENABLE;
	config.fRtsControl = RTS_CONTROL_ENABLE;
	SetCommState(serial, &config);

	COMMTIMEOUTS timeout;
	GetCommTimeouts(serial, &timeout);
	timeout.ReadIntervalTimeout = 0;
	timeout.ReadTotalTimeoutConstant = 0;
	timeout.ReadTotalTimeoutMultiplier = 0;
	timeout.WriteTotalTimeoutConstant = 1000;
	timeout.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(serial, &timeout);

	return 0;
}

void serial_close(void)
{
}

int serial_get_fd(void)
{
	return (int)serial;
}
