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

#include "config.h"

#define NAME		"xbright"
#define VERSION	"0.4"

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
	unsigned int current = getCurrent();
	unsigned int newValue = (current+1 < MAXVALUE) ? current+1 : MAXVALUE;
#ifdef VERBOSE
	printf("New brightness: %d\n", newValue);
#endif
	commitChange(newValue);
}

void
brightDown()
{
	unsigned int current = getCurrent();
	unsigned int newValue = (current > 1) ? current-1 : 0;
#ifdef VERBOSE
	printf("New brightness: %u\n", newValue);
#endif
	commitChange(newValue);
}

void
brightSet(const char *value)
{
	unsigned int setValue = atoi(++value);
	if ((setValue < 0) || (setValue > MAXVALUE))
		error("Invalid value");
#ifdef VERBOSE
	printf("New brightness: %u\n", setValue);
#endif
	commitChange(setValue);
}

unsigned int
getCurrent()
{
	FILE* input = fopen(BRIGHTNESSFILE, "r");
	if (input == NULL)
		error("Cannot open kernel pipe");
	char value = fgetc(input);
	return atoi(&value);
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
	printf("%s v%s (works only with 2.6+ kernels). Alex Kozadaev (c)\n", NAME, VERSION);
	printf("usage: %s [+-=[0-%d]]\n", NAME, MAXVALUE);
}
