#pragma once
#include "global.h"
#include "main.h"

#define KV_SIZE 32
#define HASH_TAB_SIZE  10

typedef struct storage_item {
  uint8_t key[KV_SIZE];
  uint8_t value[KV_SIZE];
  struct storage_item *next;
} storage_item;

#define storage_item_cmp(a,b) ( strncmp((char*)a->key, (char*)b->key, 32) )
SGLIB_DEFINE_LIST_PROTOTYPES(storage_item, storage_item_cmp, next)

unsigned int storage_item_hash(storage_item *i);
SGLIB_DEFINE_HASHED_CONTAINER_PROTOTYPES(storage_item, HASH_TAB_SIZE, storage_item_hash)



void storage_init(storage_item ***s);
void storage_destroy(storage_item ***s);
bool storage_read(storage_item **s, uint8_t *key, uint8_t* value);
bool storage_write(storage_item **s, uint8_t *key, uint8_t *value);
bool storage_load(storage_item **s, msgpack_object *obj);
bool storage_save(storage_item **s, msgpack_packer *pk);
