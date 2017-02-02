/*
 *  xbright.c
 *  author: Alex Kozadaev (2011-2017)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "build_host.h"

#define NAME    "xbright"

#define error(msg) { printf("ERROR: %s\n",(msg)); exit(1);}

void brightUp(void);
void brightDown(void);
void brightSet(const char*);
unsigned int getCurrent(void);
void commitChange(const unsigned int);
void usage(void);

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
brightUp(void)
{
    unsigned int current = getCurrent();
    unsigned int newValue = (current+1 < MAXVALUE) ? current+1 : MAXVALUE;
#ifdef VERBOSE
    printf("New brightness: %d\n", newValue);
#endif
    if (newValue != current) {
        commitChange(newValue);
    }
}

void
brightDown(void)
{
    unsigned int current, newValue;
    current = getCurrent();
    if (current > MAXVALUE) {
        return;  /* error in getCurrent */
    }
    newValue = (current > 1) ? current-1 : 0;
#ifdef VERBOSE
    printf("New brightness: %u\n", newValue);
#endif

    if (newValue != current) {
        commitChange(newValue);
    }
}

void
brightSet(const char *value)
{
    unsigned int setValue = atoi(++value);
    if ((setValue < 0) || (setValue > MAXVALUE)) {
        error("Invalid value");
    }
#ifdef VERBOSE
    printf("New brightness: %u\n", setValue);
#endif
    commitChange(setValue);
}

unsigned int
getCurrent(void)
{
    int value;
    FILE* input = fopen(BRIGHTNESSFILE, "r");
    if (input == NULL) {
        error("Cannot open kernel pipe");
    }
    fscanf(input, "%d", &value);
#ifdef VERBOSE
    printf("Current %d ", value);
#endif
    return value;
}

void
commitChange(unsigned const int value)
{
    FILE* output = fopen(BRIGHTNESSFILE, "w");
    if (output == NULL) {
        error("Cannot open kernel pipe");
    }
    fprintf(output, "%d", value);
    fclose(output);
}

void
usage(void)
{
    printf("%s v%s (works only with 2.6+ kernels).\n", NAME, VERSION);
    printf("usage: %s [+-=[0-%d]]\n", NAME, MAXVALUE);
}
