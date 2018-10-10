#include "util.h"
#include "env.h"
#include "storage.h"
#include "executor.h"

#define HTONLL(x) ((1==htonl(1)) ? (x) : (((uint64_t)htonl((x) & 0xFFFFFFFFUL)) << 32) | htonl((uint32_t)((x) >> 32)))
#define NTOHLL(x) ((1==ntohl(1)) ? (x) : (((uint64_t)ntohl((x) & 0xFFFFFFFFUL)) << 32) | ntohl((uint32_t)((x) >> 32)))

void exported_read(Module *m){
  exec_data *d = (exec_data*)m->extra;

  int a1 = m->stack[m->sp - 1].value.int32;
  int a2 = m->stack[m->sp - 0].value.int32;

  storage_read(d->s, m->memory.bytes + a1, m->memory.bytes + a2);
  m->sp -= 2;
}

void exported_write(Module *m){
  exec_data *d = (exec_data*)m->extra;

  int a1 = m->stack[m->sp - 1].value.int32;
  int a2 = m->stack[m->sp - 0].value.int32;

  storage_write(d->s, m->memory.bytes + a1, m->memory.bytes + a2);
  m->sp -= 2;
}


void exported_get_param(Module *m){
  exec_data *d = (exec_data*)m->extra;

  int param = m->stack[m->sp - 2].value.int32;
  size_t len = m->stack[m->sp - 1].value.int32;
  uint8_t *ptr = m->memory.bytes + m->stack[m->sp - 0].value.int32;

  msgpack_object *obj = &d->args->via.array.ptr[param];
  size_t size = msgpack_sizeof(obj);
  if (len != size){
    warn("Param size mismatch on param #%d (%zu vs %zu)!\n", param, len, size);
  }
  uint8_t v;

  // TODO: check len
  switch(obj->type) {
    case MSGPACK_OBJECT_NIL:
      memset(ptr, 0, size);
      debug("write NIL\n");
      break;
    case MSGPACK_OBJECT_BOOLEAN:
      v = obj->via.boolean ? 1 : 0;
      memset(ptr, v, size);
      debug("write bool %zu\n", size);
      break;

    case MSGPACK_OBJECT_POSITIVE_INTEGER:
      obj->via.u64 = HTONLL(obj->via.u64);
      memcpy(ptr, &obj->via.u64, size);
      debug("write int %zu\n", size);
      break;

    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
      obj->via.i64 = HTONLL(obj->via.i64);
      memcpy(ptr, &obj->via.i64, size);
      debug("write uint %zu\n", size);
      break;

    case MSGPACK_OBJECT_STR:
      memcpy(ptr, obj->via.str.ptr, size);
      debug("write str %zu\n", size);
      break;

    case MSGPACK_OBJECT_BIN:
      memcpy(ptr, obj->via.bin.ptr, size);
      debug("write bin %zu\n", size);
      break;
    default:
      break;
  }

  m->sp -= 3;
}

void exported_set_return(Module *m){
  (void)m;
}

void exported_get_sender(Module *m){
  exec_data *d = (exec_data*)m->extra;

  uint8_t *ptr = m->memory.bytes + m->stack[m->sp - 0].value.int32;
  msgpack_object *s = msgpack_get_value(d->tx, "f");
  memcpy(ptr, s->via.bin.ptr, s->via.bin.size);
  m->sp -= 1;
}

void exported_get_receiver(Module *m){
  exec_data *d = (exec_data*)m->extra;

  uint8_t *ptr = m->memory.bytes + m->stack[m->sp - 0].value.int32;
  msgpack_object *s = msgpack_get_value(d->tx, "to");
  memcpy(ptr, s->via.bin.ptr, s->via.bin.size);
  m->sp -= 1;
}

void exported_get_tx_raw_size(Module *m){
  exec_data *d = (exec_data*)m->extra;

  m->sp += 1;
  StackValue *s = &m->stack[m->sp];
  s->value_type = I32;
  s->value.int32 = d->tx_body->via.bin.size;
}

void exported_get_tx_raw(Module *m){
  exec_data *d = (exec_data*)m->extra;

  uint8_t *ptr = m->memory.bytes + m->stack[m->sp - 0].value.int32;
  memcpy(ptr, d->tx_body->via.bin.ptr, d->tx_body->via.bin.size);
  m->sp -= 1;
}

void exported_make_tx(Module *m){
  (void)m;
}

typedef struct ExportedFunc {
  char *module;
  char *name;
  void *ptr;
} ExportedFunc;

ExportedFunc FUNCS[] = {
  {"env", "storage_read", (void*)exported_read},
  {"env", "storage_write", (void*)exported_write},
  {"env", "get_param", (void*)exported_get_param},
  {"env", "set_return", (void*)exported_set_return},
  {"env", "get_sender", (void*)exported_get_sender},
  {"env", "get_receiver", (void*)exported_get_receiver},
  {"env", "get_tx_raw_size", (void*)exported_get_tx_raw_size},
  {"env", "get_tx_raw", (void*)exported_get_tx_raw},
  {"env", "make_tx", (void*)exported_make_tx},
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
