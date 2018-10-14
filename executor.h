#pragma once
#include "global.h"
#include "storage.h"

typedef struct {
  int type;
  size_t size;
  char *ptr;
} tlv;

typedef struct {
  msgpack_object *ledger;
  msgpack_unpacked uledger;

  msgpack_object *tx_container;
  msgpack_unpacked utx_container;

  msgpack_object *tx_body;
  msgpack_object *tx;
  msgpack_unpacked utx;

  msgpack_object *code;
  char *method;
  msgpack_object *args;
  msgpack_object *gas;

  storage_item **s;

  size_t new_txs_count;
  tlv new_txs[16];
  size_t ret_count;
  tlv ret[16];
} exec_data;

void do_exec(app_state *cfg, in_message *msg);
