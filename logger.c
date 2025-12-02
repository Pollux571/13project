#include "logger.h"

#include <string.h>
#include <time.h>

const char *level_names[] = {"DEBUG", "TRACE", "INFO", "WARNING", "ERROR"};

FILE *log_init(const char *filename) { return fopen(filename, "a"); }

int logcat(FILE *log_file, const char *message, enum log_level level) {
    if (!log_file || level < 0 || level > 4) return 1;

    time_t t = time(NULL);
    const struct tm *tm_info = localtime(&t);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log_file, "[%s] %s %s\n", level_names[level], time_str, message);
    fflush(log_file);

    return 0;
}

int log_close(FILE *log_file) {
    if (!log_file) return 1;
    return fclose(log_file);
}
