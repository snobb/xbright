#define MAX_REC_LEN 255

//#define VERBOSE

/* this has been tested on 2.6.33-36 kernels */
#define BRIGHTNESSFILE "/proc/acpi/video/OVGA/DD03/brightness"
#define SRCHSTR "current:"

/* insert values from "cat /proc/acpi/video/..../brightness". I think this values should work, though. */
const unsigned short levels[] = {10, 25, 35, 50, 60, 75, 90, 100};

