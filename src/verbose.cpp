// Name        : verbose.h
// Author      : Marco Boeris Frusca and Ferdinando Ricchiuti
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Functions to handle the verbose output
//============================================================================
#include "verbose.h"

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>

/*****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
/// Verbose level.
static int g_verbose = 0;

/// Verbose message label array
static char *g_verbosity_msg[] =
{
    (char*) "ERROR",
    (char*) "INFO",
    (char*) "DEBUG"
};

/*****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
void v_log(int level, char *file, int line, const char *func, char *fmt, ...)
{
    va_list args;
    time_t t;
    char time_str[16];
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);

    strftime(time_str,16,"%b %d %H:%M:%S",tm);

    if (level <= g_verbose) {
        va_start(args, fmt);
        fprintf(stderr, "[%s] %s %s:%d (%s) ", g_verbosity_msg[level], time_str, file, line, func);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
}

void v_verbosity(int level)
{
    g_verbose = level;
}

int v_required_by_verbosity(int level)
{
    return (level <= g_verbose);
}
