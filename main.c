#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <signal.h>

#include <getopt.h>

#include "main.h"
#include "socket.h"
#include "executor.h"

out_message *create_hello(uint32_t req_id) {

  out_message *msg = out_message_new();

  msg->seq = req_id;

  msgpack_packer pk;            /* packer */

  msgpack_packer_init(&pk, &msg->data, msgpack_sbuffer_write);
  msgpack_pack_map(&pk, 3);

  msgpack_pack_nil(&pk);
  msgpack_pack_str(&pk, 5);
  msgpack_pack_str_body(&pk, "hello", 5);

  msgpack_pack_str(&pk, 4);
  msgpack_pack_str_body(&pk, "lang", 4);
  msgpack_pack_str(&pk, 4);
  msgpack_pack_str_body(&pk, "wasm", 4);

  msgpack_pack_str(&pk, 3);
  msgpack_pack_str_body(&pk, "ver", 3);
  msgpack_pack_uint32(&pk, 2);

  return msg;
}

int main(int argc, char **argv) {
  char *host = "127.0.0.1";
  char *port = "5555";
  int c;

  while ((c = getopt(argc, argv, "h:p:")) != -1) {
    switch (c) {
    case 'h':
      host = optarg;
      break;
    case 'p':
      port = optarg;
      break;
    default:
      fprintf(stderr, "Usage: %s [-h host] [-p port]\n", argv[0]);
      exit(1);
    }
  }

  debug("Starting app\n");

  app_state acfg, *cfg = &acfg;

  memset(&cfg->addr, '0', sizeof(cfg->addr));
  cfg->addr.sin_family = AF_INET;
  cfg->addr.sin_port = htons(atoi(port));
  if (inet_pton(AF_INET, host, &cfg->addr.sin_addr) <= 0) {
    FATAL("inet_pton error occured\n");
    return 1;
  }

  if ((cfg->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    FATAL("could not create socket \n");
    return 2;
  }

  if (connect(cfg->socket, (struct sockaddr *) &cfg->addr, sizeof(cfg->addr)) < 0) {
    FATAL("connect failed \n");
    close(cfg->socket);
    return 3;
  }

  debug("Sending HELLO message\n");
  message_write(cfg->socket, create_hello(0));

  in_message *msg = message_read(cfg->socket);

  if (msg && msg->seq == 1) {
    msgpack_object *v = msgpack_get_value(msg->data, NULL); // TODO: check hello reply
    if (msgpack_strcmp(v, "hello") == 0) {
      debug("Hello ok\n");
      in_message_free(msg);

      while( (msg = message_read(cfg->socket)) ){
        if((msg->seq & 0x01) == 0) {
          msgpack_object *cmd = msgpack_get_value(msg->data, NULL);
          if (cmd == NULL){
            warn("Message without command\n");
            continue;
          }
          if(msgpack_strcmp(cmd, "exec") == 0){
            do_exec(cfg, msg);
          }
          if(msgpack_strcmp(cmd, "quit") == 0){
            in_message_free(msg);
            break;
          }
        }else{
          warn("unexpected response\n");
        }
        in_message_free(msg);
      }

    }else{
      debug("Hello error\n");
      in_message_free(msg);
    }
  } else {
    close(cfg->socket);
    FATAL("hello seq mismatch\n");
  }
  close(cfg->socket);
  return 0;
}
