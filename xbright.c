/*
 *  xbright.c
 *  author: Aleksei Kozadaev (2011-Present)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "build_host.h"

#define NAME "xbright"

#define ERROR(msg)      { fprintf(stderr, "ERROR: %s - %s\n", __func__, \
                                      (msg)); exit(EXIT_FAILURE); } while(0)
#ifdef DEBUG
    #define DBG(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__);
#else
    #define DBG(fmt, ...)
#endif

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))

const int STEPS = 10;
#define TOSTEPS(a)      ((int)((float)a / STEPS))
const int STEP = MAX(TOSTEPS(MAXVALUE), 1);

static void bright_up(const char *);
static void bright_down(const char *);
static void bright_set(const char *);
static int get_current(void);
static void commit_change(int);
static void usage(void);

int main(int argc, char **argv) {
    if (geteuid()) {
        usage();
        ERROR("Must be root");
        return 1;
    }

    if (argc > 1) {
        switch (*argv[1]) {
            case '+': bright_up(++argv[1]); break;
            case '-': bright_down(++argv[1]); break;
            case '=': bright_set(++argv[1]); break;
            default: usage();
        }
    } else {
        usage();
    }
    return EXIT_SUCCESS;
}

void bright_up(const char *value) {
    int offset_value = MAX(atoi(value), 1);
    int cur_value = get_current();
    int new_value = MIN(cur_value + (offset_value * STEP), MAXVALUE);

    if (new_value != cur_value) {
        commit_change(new_value);
    }
}

void bright_down(const char *value) {
    int offset_value = MAX(atoi(value), 1);
    int cur_value = get_current();
    int new_value = MAX(cur_value - (offset_value * STEP), 0);

    if (new_value != cur_value) {
        commit_change(new_value);
    }
}

void bright_set(const char *value) {
    int offset;

    if (strlen(value) == 0) {
        offset = MAXVALUE;
    } else {
        offset = MIN(atoi(value), MAXVALUE);
    }

    commit_change(offset);
}

int get_current(void) {
    int value = 0;
    FILE *input = fopen(BRIGHTNESSFILE, "r");
    if (input == NULL) {
        ERROR("Cannot open kernel pipe");
    }
    (void)fscanf(input, "%d", &value);
    DBG("current value: %d", value);
    return value;
}

void commit_change(int value) {
    FILE *output = fopen(BRIGHTNESSFILE, "w");
    if (output == NULL) {
        ERROR("Cannot open kernel pipe");
    }
    DBG("new value: %d", value);
    fprintf(output, "%d", value);
    fclose(output);
}

void usage(void) {
    printf("%s v%s (works only with 2.6+ kernels).\n", NAME, VERSION);
    printf("usage: %s [+-=[0-%d]]\n", NAME, STEPS);
}
