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
#include <stdio.h>
#include <stdlib.h>

// args are
// path to adk2tool (without .exe)
// serial port
// path to bossac (without .exe)
// binary to flash

int main(int argc, char **argv)
{
	char strbuf[1024];

	if (argc < 5) {
		fprintf(stderr, "not enough args\n");
		return 1;
	}

	snprintf(strbuf, sizeof(strbuf), "%s.exe %s erase", argv[1], argv[2]);
	puts(strbuf);
	fflush(stdout);
	system(strbuf);

	snprintf(strbuf, sizeof(strbuf), "%s.exe --port=%s -w -b %s", argv[3], argv[2], argv[4]);
	puts(strbuf);
	fflush(stdout);
	system(strbuf);

	snprintf(strbuf, sizeof(strbuf), "%s.exe %s reset", argv[1], argv[2]);
	puts(strbuf);
	fflush(stdout);
	system(strbuf);

	return 0;
}
