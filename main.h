#pragma once

#include "global.h"

typedef struct in_message {
    uint32_t seq;
    msgpack_object *data;
    msgpack_unpacker *unp;
    msgpack_unpacked und;
    struct in_message *prev;
    struct in_message *next;
} in_message;

typedef struct out_message {
    uint32_t seq;
    msgpack_sbuffer data;
    struct out_message *prev;
    struct out_message *next;
} out_message;

typedef struct app_state {
    struct sockaddr_in addr;
    int socket;
    in_message *in;
    pthread_mutex_t in_lock;
    out_message *out;
    pthread_mutex_t out_lock;
} app_state;

