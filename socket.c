

#include "util.h"
#include "socket.h"

ssize_t socket_readall(int sock, uint8_t *buffer, size_t len) {
    uint8_t *ptr = buffer;
    ssize_t actual_len = 0, n;

    while (len > 0 && (n = read(sock, ptr, len)) > 0) {
        actual_len += n;
        ptr += n;
        len -= n;
    }
    if (n == 0) {
        debug("readall n==0");
    }
    return actual_len;
}

ssize_t socket_writeall(int sock, uint8_t *buffer, size_t len) {
    uint8_t *ptr = buffer;
    ssize_t actual_len = 0, n;

    while (len > 0 && (n = write(sock, ptr, len)) > 0) {
        actual_len += n;
        ptr += n;
        len -= n;
    }
    if (n == 0) {
        debug("writeall n==0");
    }
    return actual_len;
}

in_message *in_message_new(size_t len) {
    in_message *msg = calloc(1, sizeof(in_message));

    msg->unp = msgpack_unpacker_new(len + 16);
    msgpack_unpacked_init(&msg->und);
    return msg;
}

void in_message_free(in_message *msg) {
    msgpack_unpacker_free(msg->unp);
    msgpack_unpacked_destroy(&msg->und);
    free(msg);
}

out_message *out_message_new() {
    out_message *msg = calloc(1, sizeof(out_message));

    msgpack_sbuffer_init(&msg->data);
    return msg;
}

void out_message_free(out_message *msg) {
    msgpack_sbuffer_destroy(&msg->data);
    free(msg);
}

in_message *message_read(int socket) {
    ssize_t l, len, seq;

    l = socket_readall(socket, (uint8_t *) &len, 4);
    if (l != 4) {
        warn("Unable to read msg len\n");
        return NULL;
    }
    len = ntohl((uint32_t)len);
    len -= 4;
    l = socket_readall(socket, (uint8_t *) &seq, 4);
    if (l != 4) {
        warn("Unable to read msg seq\n");
        return NULL;
    }
    seq = ntohl((uint32_t)seq);

    in_message *msg = in_message_new((size_t)len);

    msg->seq = (uint32_t)seq;

    l = socket_readall(socket, (uint8_t *) msgpack_unpacker_buffer(msg->unp), (size_t)len);
    if (l != len) {
        warn("Unable to read msg body\n");
        in_message_free(msg);
        return NULL;
    }
    msgpack_unpacker_buffer_consumed(msg->unp, (size_t)l);

    msgpack_unpack_return ret = msgpack_unpacker_next(msg->unp, &msg->und);

    switch (ret) {
    case MSGPACK_UNPACK_SUCCESS:
    case MSGPACK_UNPACK_EXTRA_BYTES:
        msg->data = &msg->und.data;
        debug("Read message seq %d\n", msg->seq);
        return msg;
    case MSGPACK_UNPACK_CONTINUE:
    case MSGPACK_UNPACK_PARSE_ERROR:
    case MSGPACK_UNPACK_NOMEM_ERROR:
        return NULL;
    }
    return NULL;
}

ssize_t message_write(int socket, out_message *msg) {
    ssize_t l, len, seq;
    len = htonl((uint32_t)msg->data.size + 4);
    socket_writeall(socket, (uint8_t *) &len, 4);
    seq = htonl(msg->seq);
    socket_writeall(socket, (uint8_t *) &seq, 4);
    l = socket_writeall(socket, (uint8_t *) msg->data.data, msg->data.size);
    out_message_free(msg);
    return l + 8;
}
