#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>             // for CHAR_BIT
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "util.h"

uint64_t read_LEB_(uint8_t *bytes, uint32_t *pos, uint32_t maxbits, bool sign) {
    uint64_t result = 0;
    uint32_t shift = 0;
    uint32_t bcnt = 0;
    uint32_t startpos = *pos;
    uint64_t byte;

    while (true) {
        byte = bytes[*pos];
        *pos += 1;
        result |= ((byte & 0x7f) << shift);
        shift += 7;
        if ((byte & 0x80) == 0) {
            break;
        }
        bcnt += 1;
        if (bcnt > (maxbits + 7 - 1) / 7) {
            FATAL("Unsigned LEB at byte %d overflow", startpos);
        }
    }
    if (sign && (shift < maxbits) && (byte & 0x40)) {
        // Sign extend
        result |= -(1 << shift);
    }
    return result;
}

uint64_t read_LEB(uint8_t *bytes, uint32_t *pos, uint32_t maxbits) {
    return read_LEB_(bytes, pos, maxbits, false);
}

uint64_t read_LEB_signed(uint8_t *bytes, uint32_t *pos, uint32_t maxbits) {
    return read_LEB_(bytes, pos, maxbits, true);
}

uint32_t read_uint32(uint8_t *bytes, uint32_t *pos) {
    *pos += 4;
    return ((uint32_t *) (bytes + *pos - 4))[0];
}

// Reads a string from the bytes array at pos that starts with a LEB length
// if result_len is not NULL, then it will be set to the string length
char *read_string(uint8_t *bytes, uint32_t *pos, uint32_t *result_len) {
    uint32_t str_len = read_LEB(bytes, pos, 32);
    char *str = malloc(str_len + 1);

    memcpy(str, bytes + *pos, str_len);
    str[str_len] = '\0';
    *pos += str_len;
    if (result_len) {
        *result_len = str_len;
    }
    return str;
}

// open and mmap a file
uint8_t *mmap_file(char *path, uint32_t *len) {
    int fd;
    struct stat sb;
    uint8_t *bytes;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        FATAL("could not open file '%s'\n", path);
    }
    if (fstat(fd, &sb) < 0) {
        FATAL("could stat file '%s'\n", path);
    }

    bytes = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (len) {
        *len = sb.st_size;          // Return length if requested
    }
    if (bytes == MAP_FAILED) {
        FATAL("could not mmap file '%s'", path);
    }
    return bytes;
}

// Assert calloc
void *acalloc(size_t nmemb, size_t size, char *name) {
    void *res = calloc(nmemb, size);

    if (res == NULL) {
        FATAL("Could not allocate %ld bytes for %s", nmemb * size, name);
    }
    return res;
}

// Assert realloc
void *arecalloc(void *ptr, size_t old_nmemb, size_t nmemb, size_t size, char *name) {
    void *res = realloc(ptr, nmemb * size);

    if (res == NULL) {
        FATAL("Could not allocate %ld bytes for %s", nmemb * size, name);
    }
    // Initialize new memory
    memset(&((uint8_t *) res)[old_nmemb * size], 0, (nmemb - old_nmemb) * size);
    return res;
}

// Split a space separated strings into an array of strings
// Returns 0 on failure
// Memory must be freed by caller
// Based on: http://stackoverflow.com/a/11198630/471795
char **split_string(char *str, int *count) {
    char **res = NULL;
    char *p = strtok(str, " ");
    int idx = 0;

    // split string and append tokens to 'res'
    while (p) {
        res = realloc(res, sizeof(char *) * idx + 1);
        if (res == NULL) {
            return 0;
        }

        res[idx++] = p;
        p = strtok(NULL, " ");
    }

    /* realloc one extra element for the last NULL */

    res = realloc(res, sizeof(char *) * (idx + 1));
    res[idx] = 0;

    if (count) {
        *count = idx;
    }
    return res;
}

// Math

// Inplace sign extend
void sext_8_32(uint32_t *val) {
    if (*val & 0x80) {
        *val = *val | 0xffffff00;
    }
}

void sext_16_32(uint32_t *val) {
    if (*val & 0x8000) {
        *val = *val | 0xffff0000;
    }
}

void sext_8_64(uint64_t *val) {
    if (*val & 0x80) {
        *val = *val | 0xffffffffffffff00;
    }
}

void sext_16_64(uint64_t *val) {
    if (*val & 0x8000) {
        *val = *val | 0xffffffffffff0000;
    }
}

void sext_32_64(uint64_t *val) {
    if (*val & 0x80000000) {
        *val = *val | 0xffffffff00000000;
    }
}

// Based on: http://stackoverflow.com/a/776523/471795
uint32_t rotl32(uint32_t n, unsigned int c) {
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

    c = c % 32;
    c &= mask;
    return (n << c) | (n >> ((-c) & mask));
}

uint32_t rotr32(uint32_t n, unsigned int c) {
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

    c = c % 32;
    c &= mask;
    return (n >> c) | (n << ((-c) & mask));
}

uint64_t rotl64(uint64_t n, unsigned int c) {
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

    c = c % 64;
    c &= mask;
    return (n << c) | (n >> ((-c) & mask));
}

uint64_t rotr64(uint64_t n, unsigned int c) {
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

    c = c % 64;
    c &= mask;
    return (n >> c) | (n << ((-c) & mask));
}

double wa_fmax(double a, double b) {
    double c = fmax(a, b);

    if (c == 0 && a == b) {
        return signbit(a) ? b : a;
    }
    return c;
}

double wa_fmin(double a, double b) {
    double c = fmin(a, b);

    if (c == 0 && a == b) {
        return signbit(a) ? a : b;
    }
    return c;
}

// MsgPack

void _dump_object(msgpack_object *obj) {
    msgpack_object_print(stdout, *obj);
}


int32_t msgpack_strcmp(msgpack_object *s1, char *s2) {
    if (s1 == NULL || s2 == NULL)
        return -1;
    if (s1->type == MSGPACK_OBJECT_STR)
        return strncmp(s1->via.str.ptr, s2, s1->via.str.size);
    if (s1->type == MSGPACK_OBJECT_BIN)
        return strncmp(s1->via.bin.ptr, s2, s1->via.bin.size);
    return -1;
}

msgpack_object *msgpack_get_value(msgpack_object *obj, char *key) {
    msgpack_object *k;

    if (obj->type == MSGPACK_OBJECT_MAP) {
        for (uint32_t i = 0; i < obj->via.map.size; ++i) {
            k = &obj->via.map.ptr[i].key;
            if (key == NULL) {
                if (k->type == MSGPACK_OBJECT_NIL)
                    return &obj->via.map.ptr[i].val;
            } else {
                if (msgpack_strcmp(k, key) == 0)
                    return &obj->via.map.ptr[i].val;
            }
        }
    }
    return NULL;
}

size_t msgpack_sizeof(msgpack_object *obj) {
    switch (obj->type) {
    case MSGPACK_OBJECT_NIL:
        return 0;
    case MSGPACK_OBJECT_BOOLEAN:
        return sizeof(bool);
    case MSGPACK_OBJECT_POSITIVE_INTEGER:
        return sizeof(uint64_t);
    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
        return sizeof(int64_t);
    case MSGPACK_OBJECT_STR:
        return obj->via.str.size;
    case MSGPACK_OBJECT_BIN:
        return obj->via.bin.size;
    default:
        return 0;
    }
}

void msgpack_pack(msgpack_packer *pk, msgpack_object *value) {
    if (value == NULL) {
        msgpack_pack_nil(pk);
        return;
    }
    switch (value->type) {
    case MSGPACK_OBJECT_NIL:
        msgpack_pack_nil(pk);
        break;
    case MSGPACK_OBJECT_BOOLEAN:
        if (value->via.boolean)
            msgpack_pack_true(pk);
        else
            msgpack_pack_false(pk);
        break;
    case MSGPACK_OBJECT_POSITIVE_INTEGER:
        msgpack_pack_uint64(pk, value->via.u64);
        break;
    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
        msgpack_pack_int64(pk, value->via.i64);
        break;
    case MSGPACK_OBJECT_STR:
        msgpack_pack_str(pk, value->via.str.size);
        msgpack_pack_str_body(pk, value->via.str.ptr, value->via.str.size);
        break;
    case MSGPACK_OBJECT_BIN:
        msgpack_pack_bin(pk, value->via.bin.size);
        msgpack_pack_bin_body(pk, value->via.bin.ptr, value->via.bin.size);
        break;
    case MSGPACK_OBJECT_EXT:
        msgpack_pack_ext(pk, value->via.ext.size, value->via.ext.type);
        msgpack_pack_ext_body(pk, value->via.ext.ptr, value->via.ext.size);
        break;
    case MSGPACK_OBJECT_ARRAY:
        msgpack_pack_array(pk, value->via.array.size);
        for (unsigned i = 0; i < value->via.array.size; i++)
            msgpack_pack(pk, &value->via.array.ptr[i]);
        break;
    case MSGPACK_OBJECT_MAP:
        msgpack_pack_map(pk, value->via.map.size);
        for (unsigned i = 0; i < value->via.map.size; i++) {
            msgpack_pack(pk, &value->via.map.ptr[i].key);
            msgpack_pack(pk, &value->via.map.ptr[i].val);
        }
        break;
    default:
        return;
    }
}

void msgpack_repack(msgpack_packer *pk, msgpack_object *value, char *key) {
    size_t s = strlen(key);
    msgpack_object *obj = msgpack_get_value(value, key);
    if (obj) {
        msgpack_pack_str(pk, s);
        msgpack_pack_str_body(pk, key, s);

        msgpack_pack(pk, obj);
    } else {
        debug("No value '%s'\n", key);
    }
}












