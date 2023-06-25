#ifndef PLOG_H
#define PLOG_H

#include <stdio.h>

#define LOG(x, ...) \
do { \
printf("[untitled:log] "x"\n", ##__VA_ARGS__); \
} while(0)

#define ERR(x, ...) \
do { \
printf("[untitled:error] "x"\n", ##__VA_ARGS__); \
} while(0)

#ifdef DEVBUILD
#define DEVLOG(x, ...) \
do { \
printf("[untitled:debug] "x"\n", ##__VA_ARGS__); \
} while(0)
#else
#define DEVLOG(x, ...)
#endif

#endif
