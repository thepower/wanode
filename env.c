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
  if ( (offset+size) >= max_offset ){
    warn("Memory owerflow\n");
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

  uint8_t *key = get_mem_ptr(m, STACK(1), KV_SIZE );
  uint8_t *val = get_mem_ptr(m, STACK(0), KV_SIZE );

  storage_read(d->s, key, val);
  m->sp -= 2;
}

void exported_write(Module *m){
  exec_data *d = (exec_data*)m->extra;

  uint8_t *key = get_mem_ptr(m, STACK(1), KV_SIZE );
  uint8_t *val = get_mem_ptr(m, STACK(0), KV_SIZE );

  storage_write(d->s, key, val);
  m->sp -= 2;
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
  {"env", "debug", (void*)exported_debug},
  {"env", "flush", (void*)exported_flush},

  {"env", "storage_read", (void*)exported_read},
  {"env", "storage_write", (void*)exported_write},

  {"env", "get_tx_raw_size", (void*)exported_get_tx_raw_size},
  {"env", "get_tx_raw", (void*)exported_get_tx_raw},

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
