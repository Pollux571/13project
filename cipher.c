#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "logger.h"

#define MAX_PATH 2048
#define MAX_LINE 4096

// === Utility Functions ===
char caesar(char c, int shift) {
    if ('a' <= c && c <= 'z') return 'a' + (c - 'a' + shift + 26) % 26;
    if ('A' <= c && c <= 'Z') return 'A' + (c - 'A' + shift + 26) % 26;
    return c;
}

void xor_block_encrypt(char *buffer, long size) {
    const unsigned char key[8] = {0x13, 0x34, 0x57, 0x79, 0x9B, 0xBC, 0xDF, 0xF1};
    for (long i = 0; i < size; ++i) {
        buffer[i] ^= key[i % 8];
    }
}

// === ✅ Fixed function ===
void output_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        printf("n/a\n\n");
        return;
    }

    char line[MAX_LINE];
    int empty = 1;

    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
        empty = 0;
    }
    fclose(file);

    if (empty)
        printf("n/a\n\n");
    else
        printf("\n");
}

// === Menu Handlers ===
void handle_option1(char *current_path, bool *path_set, FILE *logf) {
    char filename[MAX_PATH];
    if (!fgets(filename, sizeof(filename), stdin)) {
        printf("n/a\n\n");
        return;
    }

    filename[strcspn(filename, "\n")] = '\0';
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("n/a\n\n");
        *path_set = false;
        return;
    }

    fclose(file);
    strncpy(current_path, filename, MAX_PATH - 1);
    *path_set = true;

    char logmsg[MAX_LINE];
    snprintf(logmsg, sizeof(logmsg), "Файл \"%s\" открыт", filename);
    logcat(logf, logmsg, info);

    output_file(current_path);
}

void handle_option2(const char *current_path, bool path_set, FILE *logf) {
    if (!path_set) {
        printf("n/a\n\n");
        return;
    }

    char text[MAX_LINE];
    if (!fgets(text, sizeof(text), stdin)) {
        printf("n/a\n\n");
        return;
    }

    text[strcspn(text, "\n")] = '\0';
    FILE *file = fopen(current_path, "a");
    if (!file) {
        printf("n/a\n\n");
        return;
    }

    fprintf(file, "%s\n", text);
    fclose(file);

    char logmsg[MAX_LINE];
    snprintf(logmsg, sizeof(logmsg), "Строка записана в \"%s\"", current_path);
    logcat(logf, logmsg, debug);

    output_file(current_path);
}

void process_files(const char *dir_path, int mode, int shift, FILE *logf) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("n/a\n\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0 || !S_ISREG(st.st_mode)) continue;

        const char *ext = strrchr(entry->d_name, '.');
        if (!ext) continue;

        if (strcmp(ext, ".c") == 0) {
            FILE *f = fopen(full_path, "rb");
            if (!f) continue;

            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            fseek(f, 0, SEEK_SET);

            char *content = malloc(size);
            if (!content) {
                fclose(f);
                continue;
            }

            fread(content, 1, size, f);
            fclose(f);

            if (mode == 3) {
                for (long i = 0; i < size; ++i) content[i] = caesar(content[i], shift);
                logcat(logf, "Файл зашифрован шифром Цезаря", info);
            } else if (mode == 4) {
                xor_block_encrypt(content, size);
                logcat(logf, "Файл зашифрован XOR (DES-like)", info);
            }

            f = fopen(full_path, "wb");
            if (f) {
                fwrite(content, 1, size, f);
                fclose(f);
            }
            free(content);
        } else if (strcmp(ext, ".h") == 0) {
            FILE *f = fopen(full_path, "w");
            if (f) fclose(f);
            logcat(logf, "Файл .h очищен", debug);
        }
    }

    closedir(dir);
    printf("\n");
}

void handle_option3_4(int mode, FILE *logf) {
    char dir_path[MAX_PATH];
    if (!fgets(dir_path, sizeof(dir_path), stdin)) {
        printf("n/a\n\n");
        return;
    }

    dir_path[strcspn(dir_path, "\n")] = '\0';

    int shift = 0;
    if (mode == 3) {
        char shift_buf[MAX_LINE];
        if (!fgets(shift_buf, sizeof(shift_buf), stdin)) {
            printf("n/a\n\n");
            return;
        }
        shift = atoi(shift_buf);
    }

    process_files(dir_path, mode, shift, logf);
}

// === MAIN ===
int main(void) {
    char current_path[MAX_PATH] = "";
    bool path_set = false;
    char command[MAX_LINE];
    int option = 0;

    FILE *logf = log_init("../build/cipher.log");

    while (1) {
        if (!fgets(command, sizeof(command), stdin)) break;
        option = atoi(command);

        if (option == -1)
            break;
        else if (option == 1)
            handle_option1(current_path, &path_set, logf);
        else if (option == 2)
            handle_option2(current_path, path_set, logf);
        else if (option == 3)
            handle_option3_4(3, logf);
        else if (option == 4)
            handle_option3_4(4, logf);
        else
            printf("n/a\n\n");
    }

    log_close(logf);
    return 0;
}
