#pragma once
#include "global.h"
#include "storage.h"

typedef struct {
  msgpack_object *ledger;
  msgpack_unpacked uledger;

  msgpack_object *balance;

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


  msgpack_sbuffer *tx_repack;
  msgpack_sbuffer *args_repack;
  msgpack_sbuffer *balance_repack;


  char *ret_copy;
  msgpack_object *ret;
  msgpack_unpacked uret;
} exec_data;

void do_exec(app_state *cfg, in_message *msg);
