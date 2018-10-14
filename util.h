#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <msgpack.h>

#define FATAL(...) { \
    fprintf(stderr, "Error(%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); exit(1); \
}

#define ASSERT(exp, ...) { \
    if (! (exp)) { \
        fprintf(stderr, "Assert Failed (%s:%d): ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); exit(1); \
    } \
}

#if TRACE
#  define trace(...) { fprintf(stderr, "T %10s:%4d: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fflush(NULL); }
#else
#  define trace(...) ;
#endif

#if DEBUG
#  define debug(...) { fprintf(stderr, "D %10s:%4d: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fflush(NULL); }
#else
#  define debug(...) ;
#endif

#if INFO
#  define info(...) { fprintf(stderr, "I %10s:%4d: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fflush(NULL); }
#else
#  define info(...) ;
#endif

#if WARN
#  define warn(...) { fprintf(stderr, "W %10s:%4d: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fflush(NULL); }
#else
#  define warn(...) ;
#endif

#define log(...) fprintf(stderr, __VA_ARGS__);

#define error(...) fprintf(stderr, __VA_ARGS__);

uint64_t read_LEB(uint8_t * bytes, uint32_t * pos, uint32_t maxbits);
uint64_t read_LEB_signed(uint8_t * bytes, uint32_t * pos, uint32_t maxbits);

uint32_t read_uint32(uint8_t * bytes, uint32_t * pos);

char *read_string(uint8_t * bytes, uint32_t * pos, uint32_t * result_len);

uint8_t *mmap_file(char *path, uint32_t * len);

void *acalloc(size_t nmemb, size_t size, char *name);
void *arecalloc(void *ptr, size_t old_nmemb, size_t nmemb, size_t size, char *name);
char **split_string(char *str, int *count);

// Math
void sext_8_32(uint32_t * val);
void sext_16_32(uint32_t * val);
void sext_8_64(uint64_t * val);
void sext_16_64(uint64_t * val);
void sext_32_64(uint64_t * val);
uint32_t rotl32(uint32_t n, unsigned int c);
uint32_t rotr32(uint32_t n, unsigned int c);
uint64_t rotl64(uint64_t n, unsigned int c);
uint64_t rotr64(uint64_t n, unsigned int c);
double wa_fmax(double a, double b);
double wa_fmin(double a, double b);

// MsgPack
void _dump_object(msgpack_object * obj);
int32_t msgpack_strcmp(msgpack_object * s1, char *s2);
msgpack_object *msgpack_get_value(msgpack_object * obj, char *key);
size_t msgpack_sizeof(msgpack_object *obj);
