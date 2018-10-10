#include "storage.h"


unsigned int storage_item_hash(storage_item *i){
  unsigned int hash = 5381;
  for(int c = 0; c < KV_SIZE; ++c) {
    hash = ((hash << 5) + hash) + i->key[c];
  }
  return hash;
}


SGLIB_DEFINE_LIST_FUNCTIONS(storage_item, storage_item_cmp, next)
SGLIB_DEFINE_HASHED_CONTAINER_FUNCTIONS(storage_item, HASH_TAB_SIZE, storage_item_hash)

void storage_item_print(storage_item *s){
  (void)s;
  debug("k = ");
  for(int c = 0; c < KV_SIZE; ++c)
    debug("%02X", s->key[c]);
  debug("; v = ");
  for(int c = 0; c < KV_SIZE; ++c)
    debug("%02X", s->value[c]);
  debug("\n");
}

void storage_init(storage_item ***s){
  *s = calloc(HASH_TAB_SIZE, sizeof(storage_item*));
  sglib_hashed_storage_item_init(*s);
}

void storage_destroy(storage_item ***s){
  storage_item *e;
  struct sglib_hashed_storage_item_iterator it;

  for(e=sglib_hashed_storage_item_it_init(&it,*s); e!=NULL; e=sglib_hashed_storage_item_it_next(&it)) {
    free(e);
  }

  free(*s);
  *s = NULL;
}

bool storage_read(storage_item **s, uint8_t *key, uint8_t* value){
  storage_item l, *e;
  memcpy((char*)l.key, (char*)key, KV_SIZE);
  e = sglib_hashed_storage_item_find_member(s, &l);
  if (e) {
    memcpy((char*)value, (char*)e->value, KV_SIZE);
    debug("Storage read: ");
    storage_item_print(e);
    return true;
  }else{
    memset(value, 0, KV_SIZE);
    return false;
  }
}

size_t storage_size(storage_item **s){
  size_t count = 0;
  storage_item *e;
  struct sglib_hashed_storage_item_iterator it;

  for(e=sglib_hashed_storage_item_it_init(&it,s); e!=NULL; e=sglib_hashed_storage_item_it_next(&it)) {
    count++;
  }
  return count;
}

bool storage_write(storage_item **s, uint8_t *key, uint8_t *value){
  storage_item l, *e;
  memcpy((char*)l.key, (char*)key, KV_SIZE);
  e = sglib_hashed_storage_item_find_member(s, &l);

  if (e) {
    memcpy((char*)e->value, (char*)value, KV_SIZE);
    debug("Storage update: ");
    storage_item_print(e);
  }else{
    e = calloc(1, sizeof(storage_item));
    memcpy((char*)e->key, (char*)key, KV_SIZE);
    memcpy((char*)e->value, (char*)value, KV_SIZE);
    debug("Storage write: ");
    storage_item_print(e);
    sglib_hashed_storage_item_add(s, e);
  }

  return true;
}

bool storage_load(storage_item **s, msgpack_object *obj){
  msgpack_unpack_return ret;

  debug("Loading storage\n");
  msgpack_unpacked unp;
  msgpack_unpacked_init(&unp);

  ret = msgpack_unpack_next(&unp,  obj->via.bin.ptr, obj->via.bin.size, NULL);
  if (ret != MSGPACK_UNPACK_SUCCESS) {
    debug("State unpacking error\n");
    return false;
  }
  obj = &unp.data;
  if (obj == NULL || obj->type != MSGPACK_OBJECT_MAP){
    warn("Invalid state\n");
    return false;
  }

  for(uint32_t i=0;i<obj->via.map.size; ++i){
    msgpack_object_kv kv = obj->via.map.ptr[i];
    storage_write(s, (uint8_t *)kv.key.via.bin.ptr, (uint8_t *)kv.val.via.bin.ptr);
  }

  msgpack_unpacked_destroy(&unp);
  return false;
}

bool storage_save(storage_item **s, msgpack_packer *pk){
  storage_item *e;
  struct sglib_hashed_storage_item_iterator it;
  size_t len = storage_size(s);
  debug("Saving storage (%zu)\n", len);

  msgpack_pack_map(pk, len);
  for(e=sglib_hashed_storage_item_it_init(&it,s); e!=NULL; e=sglib_hashed_storage_item_it_next(&it)) {
    msgpack_pack_bin(pk, KV_SIZE);
    msgpack_pack_bin_body(pk, e->key, KV_SIZE);
    msgpack_pack_bin(pk, KV_SIZE);
    msgpack_pack_bin_body(pk, e->value, KV_SIZE);
  }
  return false;
}


typedef  struct data {
  storage_item **s;
} data;


/*int main(){*/
  /*data d;*/

  /*storage_init(&d.s);*/

  /*storage_write(d.s, "test", "asdasd");*/
  /*storage_write(d.s, "test1", "zzzzz");*/
  /*storage_write(d.s, "test", "xx");*/

  /*char v[32];*/
  /*storage_read(d.s, "test1", v);*/
  /*printf("RR %s\n", v);*/

  /*storage_destroy(&d.s);*/
  /*return 0;*/
/*}*/
