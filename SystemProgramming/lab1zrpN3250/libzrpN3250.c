#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include "plugin_api.h"

// Глобальные константы
static const char *PLUGIN_NAME = "libzrpN3250.so";
static const char *PLUGIN_PURPOSE = "Search for specific image file types based on magic numbers";
static const char *PLUGIN_AUTHOR = "Zelenskiy Rodion Pavlovich, N3250";

#define OPT_PIC "pic"
#define BUFFER_SIZE 8  // Считываем первые 8 байт для идентификации типа файла

// Описание доступных опций плагина
static struct plugin_option options[] = {
    {
        {"pic", required_argument, 0, 0},
        "Comma-separated list of image types to search for (e.g., jpg,png,gif,bmp)"
    }
};

// Магические числа для различных форматов изображений
static const unsigned char JPG_MAGIC[] = {0xFF, 0xD8, 0xFF};
static const unsigned char PNG_MAGIC[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
static const unsigned char GIF_MAGIC[] = {0x47, 0x49, 0x46, 0x38};
static const unsigned char BMP_MAGIC[] = {0x42, 0x4D};

// Функция логирования отладочной информации
void log_debug(const char *format, ...) {
    char *debug_env = getenv("LAB1DEBUG");
    if (debug_env) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
}

// Получение информации о плагине
int plugin_get_info(struct plugin_info* ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: %s: Invalid plugin_info pointer\n", PLUGIN_NAME);
        return -1;
    }
    ppi->plugin_purpose = PLUGIN_PURPOSE;
    ppi->plugin_author = PLUGIN_AUTHOR;
    ppi->sup_opts_len = sizeof(options) / sizeof(options[0]);
    ppi->sup_opts = options;
    return 0;
}

// Проверка на соответствие магическому числу
int check_magic(const unsigned char *buffer, const unsigned char *magic, size_t magic_len) {
    return memcmp(buffer, magic, magic_len) == 0;
}

// Проверка типа файла по указанным форматам изображений
int check_file_type(const char *filename, const char *pic_types) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "ERROR: %s: Failed to open file: %s\n", PLUGIN_NAME, filename);
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    close(fd);

    if (bytes_read < 0) {
        fprintf(stderr, "ERROR: %s: Error reading file: %s\n", PLUGIN_NAME, filename);
        return -1;
    }

    // Создаем копию строки с форматами изображений
    char *pic_types_copy = strdup(pic_types);
    if (!pic_types_copy) {
        fprintf(stderr, "ERROR: %s: Memory allocation failed\n", PLUGIN_NAME);
        return -1;
    }

    // Парсим и проверяем магические числа для каждого формата изображения
    char *format_token = strtok(pic_types_copy, ",");
    while (format_token != NULL) {
        if ((strcmp(format_token, "jpg") == 0 || strcmp(format_token, "jpeg") == 0) && check_magic(buffer, JPG_MAGIC, sizeof(JPG_MAGIC))) {
            free(pic_types_copy);
            return 0;
        }
        if (strcmp(format_token, "png") == 0 && check_magic(buffer, PNG_MAGIC, sizeof(PNG_MAGIC))) {
            free(pic_types_copy);
            return 0;
        }
        if (strcmp(format_token, "gif") == 0 && check_magic(buffer, GIF_MAGIC, sizeof(GIF_MAGIC))) {
            free(pic_types_copy);
            return 0;
        }
        if (strcmp(format_token, "bmp") == 0 && check_magic(buffer, BMP_MAGIC, sizeof(BMP_MAGIC))) {
            free(pic_types_copy);
            return 0;
        }
        format_token = strtok(NULL, ",");
    }

    free(pic_types_copy);
    return 1;  // Не является указанным типом изображения
}

// Обработка файла плагином
int plugin_process_file(const char *filename, struct option in_opts[], size_t in_opts_len) {
    char *pic_types = NULL;

    // Получаем значение опции --pic
    for (size_t i = 0; i < in_opts_len; i++) {
        if (strcmp(in_opts[i].name, OPT_PIC) == 0) {
            pic_types = (char *)in_opts[i].flag;
            break;
        }
    }

    if (!filename || !pic_types) {
        log_debug("DEBUG: %s: Invalid file name or pic types not set\n", PLUGIN_NAME);
        return -1;
    }

    log_debug("DEBUG: %s: Processing file: %s with pic types: %s\n", PLUGIN_NAME, filename, pic_types);

    // Проверяем соответствие файла указанным форматам
    int result = check_file_type(filename, pic_types);
    if (result == 0) {
        log_debug("DEBUG: %s: File %s is of the desired image type\n", PLUGIN_NAME, filename);
        return 0;
    }

    log_debug("DEBUG: %s: File %s is not of the desired image type\n", PLUGIN_NAME, filename);
    return 1;
}