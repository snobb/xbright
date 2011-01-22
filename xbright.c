//      xbright.c
//
//      Copyright 2011 Alex Kozadaev <akozadaev at yahoo dot com>
//
//      Redistribution and use in source and binary forms, with or without
//      modification, are permitted provided that the following conditions are
//      met:
//
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following disclaimer
//        in the documentation and/or other materials provided with the
//        distribution.
//      * Neither the name of the  nor the names of its
//        contributors may be used to endorse or promote products derived from
//        this software without specific prior written permission.
//
//      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define VERSION	"0.3"

#define arraysize(a) sizeof(a)/sizeof(a[0])
#define error(msg) { printf("ERROR: %s\n",msg); exit(1);}

void brightUp();
void brightDown();
void brightSet(const char*);
unsigned int getCurrent();
unsigned short isNumeric(const char*);
void commitChange(const unsigned int);
void usage();

int
main(int argc, char *argv[])
{
	if (geteuid()) {
		usage();
		error("Must be root");
		return 1;
	}

	if (argc > 1) {
		switch(*argv[1]) {
			case '+': brightUp(); break;
			case '-': brightDown(); break;
			case '=': brightSet(argv[1]);break;
			default: usage();
		}
	} else {
		usage();
	}
	return 0;
}

void
brightUp()
{
	int i;
	unsigned int current = getCurrent();
	for (i=0; i < arraysize(levels); i++) {
		if (current == levels[i]) break;
	}
	if (i == arraysize(levels)-1) i--;
#ifdef VERBOSE
	printf("New brightness: %d\n", levels[i+1]);
#endif
	commitChange(levels[i+1]);
}

void
brightDown()
{
	int i;
	unsigned int current = getCurrent();
	for (i=0; i < arraysize(levels); i++) {
		if (current == levels[i]) break;
	}
	if (i == 0) i++;
#ifdef VERBOSE
	printf("New brightness: %d\n", levels[i-1]);
#endif
	commitChange(levels[i-1]);
}

void
brightSet(const char *value)
{
	int i, setValue;
	setValue = atoi(++value);
	if ((setValue < 1) || (setValue > 100))
		error("Invalid value");
	for (i = 0; i < arraysize(levels); i++) {
		if (setValue <= levels[i]) break;
	}
#ifdef VERBOSE
	printf("New brightness: %d\n", levels[i]);
#endif
	commitChange(levels[i]);
}

unsigned int
getCurrent()
{
	char output[MAX_REC_LEN] = "";
	unsigned int current = 0;
	FILE* input = fopen(BRIGHTNESSFILE, "r");
	if (input == NULL)
		error("Cannot open kernel pipe");
	while (input != NULL)
	{
		fgets(output, MAX_REC_LEN, input);
		if (strstr(output, SRCHSTR) != NULL) break;
	}

	strtok(output, " ");
	char* token = (char*)strtok(NULL, " ");
	if (token != NULL) {
		if (isNumeric(token)) {
			current = atoi(token);
		}
	}
	close(input);
	return current;
}

unsigned short
isNumeric(const char *p)
{
	if (*p) {
		char c;
		while ((c=*p++) && (c != '\n')) {
			if (!isdigit(c)) return 0;
		}
 		return 1;
	}
	return 0;
}

void
commitChange(unsigned const int value)
{
	FILE* output = fopen(BRIGHTNESSFILE, "w");
	if (output == NULL)
		error("Cannot open kernel pipe");
	fprintf(output, "%d", value);
	close(output);
}

void
usage()
{
	printf("bright v%s. Alex Kozadaev (c)\n", VERSION);
	printf("usage: bright [+-=[1-100]]\n");
}
