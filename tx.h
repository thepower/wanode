#pragma once
#include "global.h"


typedef struct tx_item {
    size_t size;
    uint8_t *tx;

    struct tx_item *next;
} tx_item;


#define tx_item_cmp(a, b) ( (a->size == b->size)?strncmp((char*)a->tx, (char*)b->tx, a->size) : ( (a->size > b->size)?1:-1) )

SGLIB_DEFINE_LIST_PROTOTYPES(tx_item, tx_item_cmp, next)
