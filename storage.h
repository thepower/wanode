#pragma once

#include "global.h"
#include "main.h"

#define HASH_TAB_SIZE  128

typedef struct storage_item {
    size_t key_size;
    uint8_t *key;
    size_t value_size;
    uint8_t *value;

    struct storage_item *next;
} storage_item;

#define storage_item_cmp(a, b) ( (a->key_size == b->key_size)?strncmp((char*)a->key, (char*)b->key, a->key_size) : ( (a->key_size > b->key_size)?1:-1) )

SGLIB_DEFINE_LIST_PROTOTYPES(storage_item, storage_item_cmp, next)

unsigned int storage_item_hash(storage_item *i);

SGLIB_DEFINE_HASHED_CONTAINER_PROTOTYPES(storage_item, HASH_TAB_SIZE, storage_item_hash)


void storage_init(storage_item ***s);

void storage_destroy(storage_item ***s);

bool storage_read(storage_item **s, size_t key_size, uint8_t *key, size_t value_size, uint8_t *value);

bool storage_write(storage_item **s, size_t key_size, uint8_t *key, size_t value_size, uint8_t *value);

size_t storage_value_size(storage_item **s, size_t key_size, uint8_t *key);

bool storage_load(storage_item **s, msgpack_object *obj);

bool storage_save(storage_item **s, msgpack_packer *pk);
