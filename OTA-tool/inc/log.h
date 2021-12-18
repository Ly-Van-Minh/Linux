#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define COLOR_RED           "\x1b[1;31m"
#define COLOR_GREEN         "\x1b[1;32m"
#define COLOR_YELLOW        "\x1b[1;33m"
#define COLOR_BLUE          "\x1b[1;34m"
#define COLOR_RESET         "\x1b[1;0m"

#ifdef __cplusplus
    extern "C"{
#endif

#define LOGE(fmt, ...)      fprintf(stderr, COLOR_RED"[E] %s:%d:%s(): " fmt COLOR_RESET, \
                                    __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LOGW(fmt, ...)      fprintf(stderr, COLOR_YELLOW"[W] %s:%d:%s(): " fmt COLOR_RESET, \
                                    __FILE__, __LINE__, __func__, ##__VA_ARGS__)
                                
#define LOGI(fmt, ...)      fprintf(stderr, COLOR_GREEN"[I] %s:%d:%s(): " fmt COLOR_RESET, \
                                    __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#ifdef __cplusplus
    }
#endif

#endif /* LOG_H */
