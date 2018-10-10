#pragma once
#include "global.h"
#include "main.h"

in_message *in_message_new(size_t len);
void in_message_free(in_message *msg);

out_message *out_message_new();
void out_message_free(out_message *msg);

size_t message_write(int socket, out_message *msg);
in_message *message_read(int socket);
