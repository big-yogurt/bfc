#ifndef UTILS_H
#define UTILS_H

#define eprintf(fmt, ...) \
    do { \
        fprintf(stderr, fmt, #__VA_ARGS__); \
    } while (0)

#endif // UTILS_H
