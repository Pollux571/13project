#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#include "log_levels.h"

FILE *log_init(const char *filename);
int logcat(FILE *log_file, const char *message, enum log_level level);
int log_close(FILE *log_file);

#endif
