#include "socket.h"
#include "executor.h"
#include "wa.h"
#include "env.h"


void exec_data_init(exec_data *d) {
    memset(d, 0, sizeof(exec_data));
    storage_init(&d->s);
}

void exec_data_destroy(exec_data *d) {
    if (d->method) {
        free(d->method);
    }
    if (d->ret_copy) {
        free(d->ret_copy);
    }
    if (d->tx_repack) {
        msgpack_sbuffer_free(d->tx_repack);
    }
    if (d->args_repack) {
        msgpack_sbuffer_free(d->args_repack);
    }
    storage_destroy(&d->s);
    msgpack_unpacked_destroy(&d->utx_container);
    msgpack_unpacked_destroy(&d->utx);
    msgpack_unpacked_destroy(&d->uledger);
    msgpack_unpacked_destroy(&d->uret);
}

bool parse_exec_data(in_message *msg, exec_data *d) {
    msgpack_object *obj;
    msgpack_object *ledger, *state, *balance, *txc, *tx, *call, *method;
    msgpack_unpack_return ret;

    if ((msg->seq & 0x01) == 0) {
        if (msgpack_strcmp(msgpack_get_value(msg->data, NULL), "exec") == 0) {

            // Unpack ledger and get stored state
            ledger = msgpack_get_value(msg->data, "ledger");
            if (ledger == NULL || ledger->type != MSGPACK_OBJECT_BIN) { warn("Exec request without ledger\n");
                return false;
            }

            ret = msgpack_unpack_next(&d->uledger, ledger->via.bin.ptr, ledger->via.bin.size, NULL);
            if (ret != MSGPACK_UNPACK_SUCCESS) { debug("Ledger unpack error\n");
                return false;
            }debug("Ledger unpacked\n");
            d->ledger = &d->uledger.data;

            balance = msgpack_get_value(d->ledger, "amount");
            if (balance == NULL || balance->type != MSGPACK_OBJECT_MAP) { warn("Ledger without state\n");
            } else {
                d->balance = balance;
            }

            state = msgpack_get_value(d->ledger, "state");
            if (state == NULL || state->type != MSGPACK_OBJECT_BIN) { warn("Ledger without state\n");
            } else {
                storage_load(d->s, state);
            }

            // Try to unpack TX
            txc = msgpack_get_value(msg->data, "tx");
            if (txc == NULL || txc->type != MSGPACK_OBJECT_BIN) { debug("Exec request without TX\n");

                // TX container missing
                d->code = msgpack_get_value(d->ledger, "code");
                if (d->code == NULL) { warn("Ledger without code\n");
                    return false;
                }debug("Generic transaction, using code from ledger\n");

                call = msgpack_get_value(msg->data, "c");
                if (call == NULL || call->type != MSGPACK_OBJECT_ARRAY) { debug(
                            "TXless call without call parameters\n");
                    return false;
                }
                method = &call->via.array.ptr[0];
                if (method == NULL || method->type != MSGPACK_OBJECT_STR) { debug("Invalid method name type\n");
                    return false;
                } else {
                    d->method = calloc(method->via.str.size + 1, 1);
                    strncat(d->method, method->via.str.ptr, method->via.str.size);
                }
                d->args = &call->via.array.ptr[1];
                if (d->args == NULL || d->args->type != MSGPACK_OBJECT_ARRAY) { debug("Invalid args type\n");
                    return false;
                }
            } else {
                // TX container present
                ret = msgpack_unpack_next(&d->utx_container, txc->via.bin.ptr, txc->via.bin.size, NULL);

                if (ret != MSGPACK_UNPACK_SUCCESS) { debug("Tx container unpack error\n");
                    return false;
                }debug("Tx container unpacked\n");
                d->tx_container = &d->utx_container.data;

                obj = msgpack_get_value(d->tx_container, "ver");
                if (obj == NULL || obj->type != MSGPACK_OBJECT_POSITIVE_INTEGER || obj->via.u64 != 0x02) { debug(
                            "Invalid tx container version %ld\n", obj->via.u64);
                    return false;
                }

                tx = msgpack_get_value(d->tx_container, "body");
                if (tx == NULL || tx->type != MSGPACK_OBJECT_BIN) { warn("TX container without TX body\n");
                    return false;
                }
                d->tx_body = tx;
                ret = msgpack_unpack_next(&d->utx, tx->via.bin.ptr, tx->via.bin.size, NULL);

                if (ret != MSGPACK_UNPACK_SUCCESS) { debug("Tx unpack error\n");
                    return false;
                }debug("Tx unpacked\n");
                d->tx = &d->utx.data;

                obj = msgpack_get_value(d->tx, "k");
                if (obj == NULL || obj->type != MSGPACK_OBJECT_POSITIVE_INTEGER) { warn("TX without Kind\n");
                    return false;
                }
                uint64_t kind = obj->via.u64;
                if (kind != 16 && kind != 18) { debug("Skipping non-interesting tx kind\n");
                    return false;
                }debug("Tx kind = %ld\n", kind);

                call = msgpack_get_value(d->tx, "c");
                if (call == NULL || call->type != MSGPACK_OBJECT_ARRAY) { debug("TX call without call parameters\n");
                    return false;
                }
                method = &call->via.array.ptr[0];
                if (method == NULL || method->type != MSGPACK_OBJECT_STR) { debug("Invalid method name type\n");
                    return false;
                } else {
                    d->method = calloc(method->via.str.size + 1, 1);
                    strncat(d->method, method->via.str.ptr, method->via.str.size);
                }
                d->args = &call->via.array.ptr[1];
                if (d->args == NULL || d->args->type != MSGPACK_OBJECT_ARRAY) { debug("Invalid args type\n");
                    return false;
                }

                switch (kind) {
                    case 16: // Generic TX
                        d->code = msgpack_get_value(d->ledger, "code");
                        if (d->code == NULL) { warn("Ledger without code\n");
                            return false;
                        }
                        if (strncmp(d->method, "init", 4) == 0) { warn("Attempt to call INIT on non-deploy TX\n");
                            return false;
                        }debug("Generic transaction, using code from ledger\n");
                        break;
                    case 18: // Deploy TX
                        obj = msgpack_get_value(d->tx, "e");
                        if (obj == NULL) { warn("Deploy transaction without Extradata\n");
                            return false;
                        }
                        obj = msgpack_get_value(obj, "code");
                        if (obj == NULL) { warn("Extradata without code\n");
                            return false;
                        }
                        d->code = obj;debug("Deploy transaction, using code from TX\n");
                        if (d->method) {
                            free(d->method);
                        }
                        d->method = calloc(5, 1);
                        strncat(d->method, "init", 4);
                        break;
                    default:debug("Skipping non-interesting tx kind\n");
                        return false;
                }
            }

            d->gas = msgpack_get_value(msg->data, "gas");

            return true;
        } else { debug("Skipping unknown message kind\n");
        }
    } else { debug("Skipping response message\n");
    }
    return false;
}

void do_exec(app_state *cfg, in_message *msg) {
    exec_data d;
    exec_data_init(&d);

    out_message *out = out_message_new();
    out->seq = msg->seq | 1;

    msgpack_packer pk;
    msgpack_packer_init(&pk, &out->data, msgpack_sbuffer_write);

    if (parse_exec_data(msg, &d)) { info("Processing request %d\n", msg->seq >> 1);
        Options opts = {false, resolvesym};
        Module *m = load_module((uint8_t *) d.code->via.bin.ptr, d.code->via.bin.size, opts);
        // FIXME:
        *(int *) (m->memory.bytes + 4) = m->memory.pages * 2 << 15;
        m->gas = d.gas->via.u64;
        m->extra = &d;debug("Module loaded\n");debug("Starting with gas = %lu\n", m->gas);

        char *name = calloc(1024, 1);
        strncat(name, d.method, 1024);
        strncat(name, "_wrapper", 8);

        bool res = invoke(m, name, 0, NULL);
        free(name);

        if (res) { debug("EXEC OK, gas = %lu\n", m->gas);

            msgpack_pack_map(&pk, 5);

            msgpack_pack_nil(&pk);
            msgpack_pack_str(&pk, 4);
            msgpack_pack_str_body(&pk, "exec", 4);

            msgpack_sbuffer data;
            msgpack_sbuffer_init(&data);
            msgpack_packer subpk;
            msgpack_packer_init(&subpk, &data, msgpack_sbuffer_write);
            storage_save(d.s, &subpk);

            msgpack_pack_str(&pk, 5);
            msgpack_pack_str_body(&pk, "state", 5);
            msgpack_pack_bin(&pk, data.size);
            msgpack_pack_bin_body(&pk, data.data, data.size);

            msgpack_sbuffer_destroy(&data);


            msgpack_pack_str(&pk, 3);
            msgpack_pack_str_body(&pk, "txs", 3);
            msgpack_pack_array(&pk, 0);

            msgpack_pack_str(&pk, 3);
            msgpack_pack_str_body(&pk, "ret", 3);
            if (d.ret) {
                msgpack_pack(&pk, d.ret);
            } else {
                msgpack_pack_nil(&pk);
            }

            msgpack_pack_str(&pk, 3);
            msgpack_pack_str_body(&pk, "gas", 3);
            msgpack_pack_fix_uint64(&pk, m->gas);
        } else { debug("EXEC ERR: %s, gas = %lu\n", exception, m->gas);

            msgpack_pack_map(&pk, 3);

            msgpack_pack_nil(&pk);
            msgpack_pack_str(&pk, 4);
            msgpack_pack_str_body(&pk, "exec", 4);

            msgpack_pack_str(&pk, 3);
            msgpack_pack_str_body(&pk, "err", 3);
            size_t l = strlen(exception);
            msgpack_pack_str(&pk, l);
            msgpack_pack_str_body(&pk, exception, l);

            msgpack_pack_str(&pk, 3);
            msgpack_pack_str_body(&pk, "gas", 3);
            msgpack_pack_fix_uint64(&pk, m->gas);
        }
        module_free(m);
    } else {
        msgpack_pack_map(&pk, 2);

        msgpack_pack_nil(&pk);
        msgpack_pack_str(&pk, 4);
        msgpack_pack_str_body(&pk, "exec", 4);

        msgpack_pack_str(&pk, 3);
        msgpack_pack_str_body(&pk, "err", 3);
        msgpack_pack_str(&pk, 15);
        msgpack_pack_str_body(&pk, "invalid request", 15);
    }
    message_write(cfg->socket, out);
    exec_data_destroy(&d);
}
