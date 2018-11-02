#include <math.h>

#include "util.h"
#include "env.h"
#include "storage.h"
#include "executor.h"

#define HTONLL(x) ((1==htonl(1)) ? (x) : (((uint64_t)htonl((x) & 0xFFFFFFFFUL)) << 32) | htonl((uint32_t)((x) >> 32)))
#define NTOHLL(x) ((1==ntohl(1)) ? (x) : (((uint64_t)ntohl((x) & 0xFFFFFFFFUL)) << 32) | ntohl((uint32_t)((x) >> 32)))

#define STACK(x) (m->stack[m->sp - x].value.int32)

uint8_t *get_mem_ptr(Module *m, int32_t offset, int32_t size) {
  int32_t max_offset = m->memory.pages * pow(2, 16);
  if ( (offset+size) > max_offset ){
    warn("Memory owerflow, %d %d %d\n", offset, size, max_offset);
    return NULL;
  }
  return m->memory.bytes + offset;
}

char Dbuf[1024];
int Dpos = 0;

void exported_flush(Module *m){
  (void)m;
  debug("CONTRACT SAID: '%s'\n", Dbuf);
  Dpos = 0;
  Dbuf[0] = 0;
}

void exported_debug(Module *m){
  size_t len = STACK(1);
  char *str = (char*)get_mem_ptr(m, STACK(0), len);
  Dpos += len;
  if (Dpos + 1 > 1024){
    exported_flush(m);
  }
  strncat(Dbuf, str, len);
  m->sp -= 2;
}

void exported_read(Module *m){
  exec_data *d = (exec_data*)m->extra;

  size_t key_size = STACK(3);
  uint8_t *key = get_mem_ptr(m, STACK(2), key_size );
  size_t value_size = STACK(1);
  uint8_t *val = get_mem_ptr(m, STACK(0), value_size );

  storage_read(d->s, key_size, key, value_size, val);
  m->sp -= 4;
}

void exported_write(Module *m){
  exec_data *d = (exec_data*)m->extra;

  size_t key_size = STACK(3);
  uint8_t *key = get_mem_ptr(m, STACK(2), key_size );
  size_t value_size = STACK(1);
  uint8_t *val = get_mem_ptr(m, STACK(0), value_size );

  storage_write(d->s, key_size, key, value_size, val);
  m->sp -= 4;
}

void exported_get_value_size(Module *m){
  exec_data *d = (exec_data*)m->extra;

  size_t key_size = STACK(1);
  uint8_t *key = get_mem_ptr(m, STACK(0), key_size );
  m->sp -= 2;

  size_t value_size = storage_value_size(d->s, key_size, key);

  m->sp += 1;
  StackValue *s = &m->stack[m->sp];
  s->value_type = I32;
  s->value.int32 = value_size;
  debug("Value size %zu\n", value_size);
}

void tx_repack(Module *m){
  exec_data *d = (exec_data*)m->extra;
  msgpack_object *tx = d->tx;

  if( tx == NULL || tx->type != MSGPACK_OBJECT_MAP ){
    return;
  }

  if(d->tx_repack != NULL ){
    return;
  }

  d->tx_repack = msgpack_sbuffer_new();
  msgpack_packer *pk = msgpack_packer_new(d->tx_repack, msgpack_sbuffer_write);

  msgpack_pack_map(pk, 5);

  msgpack_repack(pk, tx, "p");
  msgpack_repack(pk, tx, "f");
  msgpack_repack(pk, tx, "to");
  msgpack_repack(pk, tx, "k");
  msgpack_repack(pk, tx, "t");

  msgpack_packer_free(pk);
}

void exported_get_tx_raw_size(Module *m){
  exec_data *d = (exec_data*)m->extra;
  tx_repack(m);

  m->sp += 1;
  StackValue *s = &m->stack[m->sp];
  s->value_type = I32;
  if(d->tx_repack){
    s->value.int32 = d->tx_repack->size;
  }else{
    s->value.int32 = 0;
  }
  debug("tx_raw_size = %d\n", s->value.int32);
}

void exported_get_tx_raw(Module *m){
  exec_data *d = (exec_data*)m->extra;
  tx_repack(m);

  if(d->tx_repack == NULL){
    STACK(0) = 0;
    return;
  }

  uint8_t *ptr = get_mem_ptr(m, STACK(0), d->tx_repack->size);
  if(ptr){
    STACK(0) = 1;
    memcpy(ptr, d->tx_repack->data, d->tx_repack->size);
  }
}

void args_repack(Module *m){
  exec_data *d = (exec_data*)m->extra;
  msgpack_object *args = d->args;

  if( args == NULL || args->type != MSGPACK_OBJECT_ARRAY ){
    return;
  }

  if(d->args_repack != NULL ){
    return;
  }

  d->args_repack = msgpack_sbuffer_new();
  msgpack_packer *pk = msgpack_packer_new(d->args_repack, msgpack_sbuffer_write);

  msgpack_pack(pk, args) ;

  msgpack_packer_free(pk);
}

void exported_get_args_raw_size(Module *m){
  exec_data *d = (exec_data*)m->extra;
  args_repack(m);

  m->sp += 1;
  StackValue *s = &m->stack[m->sp];
  s->value_type = I32;
  if(d->args_repack){
    debug("Args RAW size = %zu\n", d->args_repack->size);
    s->value.int32 = d->args_repack->size;
  }else{
    debug("Args RAW size = 0\n");
    s->value.int32 = 0;
  }
}

void exported_get_args_raw(Module *m){
  exec_data *d = (exec_data*)m->extra;
  args_repack(m);

  if(d->args_repack == NULL){
    STACK(0) = 0;
    return;
  }

  uint8_t *ptr = get_mem_ptr(m, STACK(0), d->args_repack->size);
  if(ptr){
    STACK(0) = 1;
    memcpy(ptr, d->args_repack->data, d->args_repack->size);
  }
}

void exported_set_return(Module *m){
  exec_data *d = (exec_data*)m->extra;
  size_t len = STACK(1);
  char *ret = (char*)get_mem_ptr(m, STACK(0), len);
  m->sp -= 2;

  if(ret){
    d->ret_copy = malloc(len);
    memcpy(d->ret_copy, ret, len);
  }

  if (msgpack_unpack_next(&d->uret, d->ret_copy, len, NULL) != MSGPACK_UNPACK_SUCCESS) {
    d->ret = NULL;
  }else{
    d->ret = &d->uret.data;
  }
}



typedef struct ExportedFunc {
  char *module;
  char *name;
  void *ptr;
} ExportedFunc;

ExportedFunc FUNCS[] = {
  {"env", "debug", (void*)exported_debug},
  {"env", "flush", (void*)exported_flush},

  {"env", "storage_read", (void*)exported_read},
  {"env", "storage_write", (void*)exported_write},
  {"env", "storage_value_size", (void*)exported_get_value_size},

  {"env", "get_tx_raw_size", (void*)exported_get_tx_raw_size},
  {"env", "get_tx_raw", (void*)exported_get_tx_raw},

  {"env", "get_args_raw_size", (void*)exported_get_args_raw_size},
  {"env", "get_args_raw", (void*)exported_get_args_raw},

  {"env", "set_return", (void*)exported_set_return},

  {NULL, NULL, NULL},
};

typedef struct ExportedGlobal {
  char *module;
  char *name;
  StackValue value;
} ExportedGlobal;

ExportedGlobal GLOBALS[] = {
  {"env", "ext_x", {I32, {.int32 = 0x3e8}}},
  {"env", "ext_y", {I32, {.int32 = 0x27e}}},
  {NULL, NULL, {I32, {.int32 = 0}}},
};

bool resolvesym(char *module, char *name, void **val, char **err) {
  ExportedFunc *f = &FUNCS[0];

  debug("Resolving %s.%s\n", module, name);

  while (f->module || f->name) {
    if (!f->module || !strcmp(f->module, module)) {
      if (f->name && !strcmp(f->name, name)) {
        debug("%s.%s resolved to function\n", module, name);
        *val = f->ptr;
        return true;
      }
    }
    f++;
  }
  ExportedGlobal *g = &GLOBALS[0];

  while (g->module || g->name) {
    if (!g->module || !strcmp(g->module, module)) {
      if (g->name && !strcmp(g->name, name)) {
        debug("%s.%s resolved to global\n", module, name);
        *val = &g->value.value;
        return true;
      }
    }
    g++;
  }
  *err = "Symbol not found";
  return false;
}
