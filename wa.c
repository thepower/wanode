
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "util.h"
#include "wa.h"


char OPERATOR_INFO[][20] = {
    // Control flow operators
    "unreachable",                // 0x00
    "nop",                        // 0x01
    "block",                      // 0x02
    "loop",                       // 0x03
    "if",                         // 0x04
    "else",                       // 0x05
    "RESERVED",                   // 0x06
    "RESERVED",                   // 0x07
    "RESERVED",                   // 0x08
    "RESERVED",                   // 0x09
    "RESERVED",                   // 0x0a
    "end",                        // 0x0b
    "br",                         // 0x0c
    "br_if",                      // 0x0d
    "br_table",                   // 0x0e
    "return",                     // 0x0f

    // Call operators
    "call",                       // 0x10
    "call_indirect",              // 0x11

    "RESERVED",                   // 0x12
    "RESERVED",                   // 0x13
    "RESERVED",                   // 0x14
    "RESERVED",                   // 0x15
    "RESERVED",                   // 0x16
    "RESERVED",                   // 0x17
    "RESERVED",                   // 0x18
    "RESERVED",                   // 0x19

    // Parametric operators
    "drop",                       // 0x1a
    "select",                     // 0x1b

    "RESERVED",                   // 0x1c
    "RESERVED",                   // 0x1d
    "RESERVED",                   // 0x1e
    "RESERVED",                   // 0x1f

    // Variable access
    "get_local",                  // 0x20
    "set_local",                  // 0x21
    "tee_local",                  // 0x22
    "get_global",                 // 0x23
    "set_global",                 // 0x24

    "RESERVED",                   // 0x25
    "RESERVED",                   // 0x26
    "RESERVED",                   // 0x27

    // Memory-related operator
    "i32.load",                   // 0x28
    "i64.load",                   // 0x29
    "f32.load",                   // 0x2a
    "f64.load",                   // 0x2b
    "i32.load8_s",                // 0x2c
    "i32.load8_u",                // 0x2d
    "i32.load16_s",               // 0x2e
    "i32.load16_u",               // 0x2f
    "i64.load8_s",                // 0x30
    "i64.load8_u",                // 0x31
    "i64.load16_s",               // 0x32
    "i64.load16_u",               // 0x33
    "i64.load32_s",               // 0x34
    "i64.load32_u",               // 0x35
    "i32.store",                  // 0x36
    "i64.store",                  // 0x37
    "f32.store",                  // 0x38
    "f64.store",                  // 0x39
    "i32.store8",                 // 0x3a
    "i32.store16",                // 0x3b
    "i64.store8",                 // 0x3c
    "i64.store16",                // 0x3d
    "i64.store32",                // 0x3e
    "current_memory",             // 0x3f
    "grow_memory",                // 0x40

    // Constants
    "i32.const",                  // 0x41
    "i64.const",                  // 0x42
    "f32.const",                  // 0x43
    "f64.const",                  // 0x44

    // Comparison operators
    "i32.eqz",                    // 0x45
    "i32.eq",                     // 0x46
    "i32.ne",                     // 0x47
    "i32.lt_s",                   // 0x48
    "i32.lt_u",                   // 0x49
    "i32.gt_s",                   // 0x4a
    "i32.gt_u",                   // 0x4b
    "i32.le_s",                   // 0x4c
    "i32.le_u",                   // 0x4d
    "i32.ge_s",                   // 0x4e
    "i32.ge_u",                   // 0x4f

    "i64.eqz",                    // 0x50
    "i64.eq",                     // 0x51
    "i64.ne",                     // 0x52
    "i64.lt_s",                   // 0x53
    "i64.lt_u",                   // 0x54
    "i64.gt_s",                   // 0x55
    "i64.gt_u",                   // 0x56
    "i64.le_s",                   // 0x57
    "i64.le_u",                   // 0x58
    "i64.ge_s",                   // 0x59
    "i64.ge_u",                   // 0x5a

    "f32.eq",                     // 0x5b
    "f32.ne",                     // 0x5c
    "f32.lt",                     // 0x5d
    "f32.gt",                     // 0x5e
    "f32.le",                     // 0x5f
    "f32.ge",                     // 0x60

    "f64.eq",                     // 0x61
    "f64.ne",                     // 0x62
    "f64.lt",                     // 0x63
    "f64.gt",                     // 0x64
    "f64.le",                     // 0x65
    "f64.ge",                     // 0x66

    // Numeric operators
    "i32.clz",                    // 0x67
    "i32.ctz",                    // 0x68
    "i32.popcnt",                 // 0x69
    "i32.add",                    // 0x6a
    "i32.sub",                    // 0x6b
    "i32.mul",                    // 0x6c
    "i32.div_s",                  // 0x6d
    "i32.div_u",                  // 0x6e
    "i32.rem_s",                  // 0x6f
    "i32.rem_u",                  // 0x70
    "i32.and",                    // 0x71
    "i32.or",                     // 0x72
    "i32.xor",                    // 0x73
    "i32.shl",                    // 0x74
    "i32.shr_s",                  // 0x75
    "i32.shr_u",                  // 0x76
    "i32.rotl",                   // 0x77
    "i32.rotr",                   // 0x78

    "i64.clz",                    // 0x79
    "i64.ctz",                    // 0x7a
    "i64.popcnt",                 // 0x7b
    "i64.add",                    // 0x7c
    "i64.sub",                    // 0x7d
    "i64.mul",                    // 0x7e
    "i64.div_s",                  // 0x7f
    "i64.div_u",                  // 0x80
    "i64.rem_s",                  // 0x81
    "i64.rem_u",                  // 0x82
    "i64.and",                    // 0x83
    "i64.or",                     // 0x84
    "i64.xor",                    // 0x85
    "i64.shl",                    // 0x86
    "i64.shr_s",                  // 0x87
    "i64.shr_u",                  // 0x88
    "i64.rotl",                   // 0x89
    "i64.rotr",                   // 0x8a

    "f32.abs",                    // 0x8b
    "f32.neg",                    // 0x8c
    "f32.ceil",                   // 0x8d
    "f32.floor",                  // 0x8e
    "f32.trunc",                  // 0x8f
    "f32.nearest",                // 0x90
    "f32.sqrt",                   // 0x91
    "f32.add",                    // 0x92
    "f32.sub",                    // 0x93
    "f32.mul",                    // 0x94
    "f32.div",                    // 0x95
    "f32.min",                    // 0x96
    "f32.max",                    // 0x97
    "f32.copysign",               // 0x98

    "f64.abs",                    // 0x99
    "f64.neg",                    // 0x9a
    "f64.ceil",                   // 0x9b
    "f64.floor",                  // 0x9c
    "f64.trunc",                  // 0x9d
    "f64.nearest",                // 0x9e
    "f64.sqrt",                   // 0x9f
    "f64.add",                    // 0xa0
    "f64.sub",                    // 0xa1
    "f64.mul",                    // 0xa2
    "f64.div",                    // 0xa3
    "f64.min",                    // 0xa4
    "f64.max",                    // 0xa5
    "f64.copysign",               // 0xa6

    // Conversions
    "i32.wrap/i64",               // 0xa7
    "i32.trunc_s/f32",            // 0xa8
    "i32.trunc_u/f32",            // 0xa9
    "i32.trunc_s/f64",            // 0xaa
    "i32.trunc_u/f64",            // 0xab

    "i64.extend_s/i32",           // 0xac
    "i64.extend_u/i32",           // 0xad
    "i64.trunc_s/f32",            // 0xae
    "i64.trunc_u/f32",            // 0xaf
    "i64.trunc_s/f64",            // 0xb0
    "i64.trunc_u/f64",            // 0xb1

    "f32.convert_s/i32",          // 0xb2
    "f32.convert_u/i32",          // 0xb3
    "f32.convert_s/i64",          // 0xb4
    "f32.convert_u/i64",          // 0xb5
    "f32.demote/f64",             // 0xb6

    "f64.convert_s/i32",          // 0xb7
    "f64.convert_u/i32",          // 0xb8
    "f64.convert_s/i64",          // 0xb9
    "f64.convert_u/i64",          // 0xba
    "f64.promote/f32",            // 0xbb

    // Reinterpretations
    "i32.reinterpret/f32",        // 0xbc
    "i64.reinterpret/f64",        // 0xbd
    "f32.reinterpret/i32",        // 0xbe
    "f64.reinterpret/i64"         // 0xbf
};

// Size of memory load.
// This starts with the first memory load operator at opcode 0x28
uint32_t LOAD_SIZE[] = {
    4, 8, 4, 8, 1, 1, 2, 2, 1, 1, 2, 2, 4, 4, // loads
    4, 8, 4, 8, 1, 2, 1, 2, 4
};                              // stores

// Static definition of block_types
uint32_t block_type_results[4][1] = {{I32},
    {I64},
    {F32},
    {F64}
};

Type block_types[5] = {
    {.form = BLOCK, .result_count = 0,},
    {.form = BLOCK, .result_count = 1, .results = block_type_results[0],},
    {.form = BLOCK, .result_count = 1, .results = block_type_results[1],},
    {.form = BLOCK, .result_count = 1, .results = block_type_results[2],},
    {.form = BLOCK, .result_count = 1, .results = block_type_results[3],}
};

Type *get_block_type(uint8_t value_type) {
    switch (value_type) {
    case 0x40:
        return &block_types[0];
    case I32:
        return &block_types[1];
    case I64:
        return &block_types[2];
    case F32:
        return &block_types[3];
    case F64:
        return &block_types[4];
    default:
        FATAL("invalid block_type value_type: %d\n", value_type);
    }
}

char _value_str[256];

char *value_repr(StackValue *v) {
    switch (v->value_type) {
    case I32:
        snprintf(_value_str, 255, "0x%x:i32", v->value.uint32);
        break;
    case I64:
        snprintf(_value_str, 255, "0x%lx:i64", v->value.uint64);
        break;
    case F32:
        snprintf(_value_str, 255, "%.7g:f32", v->value.f32);
        break;
    case F64:
        snprintf(_value_str, 255, "%.7g:f64", v->value.f64);
        break;
    }
    return _value_str;
}

char _block_str[1024];

char *block_repr(Block *b) {
    if (b->block_type == 0) {
        snprintf(_block_str, 1023,
                 "fn0x%x<%d/%d->%d>", b->fidx, b->type->param_count,
                 b->local_count, b->type->result_count);
    } else {
        snprintf(_block_str, 1023, "%s<0/0->%d>",
                 b->block_type == 0x01 ? "init" :
                 b->block_type == 0x02 ? "block" :
                 b->block_type == 0x03 ? "loop" : "if", b->type->result_count);
    }
    return _block_str;
}

void dump_stacks(Module *m) {
    trace("      * stack:     [");
    for (int i = 0; i <= m->sp; i++) {
        if (i == m->fp) {
            trace("* ");
        }
        trace("%s", value_repr(&m->stack[i]));
        if (i != m->sp) {
            trace(" ");
        }
    }
    trace("]\n");

    trace("      * callstack: [");
    for (int i = 0; i <= m->csp; i++) {
        Frame *f = &m->callstack[i];
        (void) f;
        trace("%s(sp:%d/fp:%d/ra:0x%x)", block_repr(f->block), f->sp, f->fp, f->ra);
        if (i != m->csp) {
            trace(" ");
        }
    }
    trace("]\n");
}

void parse_table_type(Module *m, uint32_t *pos) {
    m->table.elem_type = read_LEB(m->bytes, pos, 7);
    ASSERT(m->table.elem_type == ANYFUNC,
           "Table elem_type 0x%x unsupported", m->table.elem_type);
    uint32_t flags = read_LEB(m->bytes, pos, 32);
    uint32_t tsize = read_LEB(m->bytes, pos, 32); // Initial size

    m->table.initial = tsize;
    m->table.size = tsize;
    // Limit maximum to 64K
    if (flags & 0x1) {
        tsize = read_LEB(m->bytes, pos, 32);  // Max size
        m->table.maximum = (uint32_t) fmin(0x10000, tsize);
    } else {
        m->table.maximum = 0x10000;
    }
}

void parse_memory_type(Module *m, uint32_t *pos) {
    uint32_t flags = read_LEB(m->bytes, pos, 32);
    uint32_t pages = read_LEB(m->bytes, pos, 32); // Initial size

    m->memory.initial = pages;
    m->memory.pages = pages;
    // Limit the maximum to 2GB
    if (flags & 0x1) {
        pages = read_LEB(m->bytes, pos, 32);  // Max size
        m->memory.maximum = (uint32_t) fmin(0x8000, pages);
    } else {
        m->memory.maximum = 0x8000;
    }
    trace("Memory min %d (%x), msx %d (%x)\n", m->memory.initial, m->memory.initial, pages,
          pages);
}

void skip_immediates(uint8_t *bytes, uint32_t *pos) {
    uint32_t count, opcode = bytes[*pos];

    *pos = *pos + 1;
    switch (opcode) {
    // varuint1
    case 0x3f:                   // current_memory, grow_memory
    case 0x40:
        read_LEB(bytes, pos, 1);
        break;
    // varuint32, varint32
    case 0x0c:
    case 0x0d:                   // br, br_if
    case 0x10:                   // call
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:                   // get/set_local, tee_local, get/set_global
    case 0x41:                   // i32.const
        read_LEB(bytes, pos, 32);
        break;
    // varuint32 + varuint1
    case 0x11:                   // call_indirect
        read_LEB(bytes, pos, 1);
        read_LEB(bytes, pos, 32);
        break;
    // varint64
    case 0x42:                   // i64.const
        read_LEB(bytes, pos, 64);
        break;
    // uint32
    case 0x43:                   // f32.const
        *pos += 4;
        break;
    // uint64
    case 0x44:                   // f64.const
        *pos += 8;
        break;
    // block_type
    case 0x02:
    case 0x03:
    case 0x04:          // block, loop, if
        read_LEB(bytes, pos, 7);
        break;
    // memory_immediate

    case 0x28: // *.load*, *.store*
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:

        read_LEB(bytes, pos, 32);
        read_LEB(bytes, pos, 32);
        break;
    // br_table
    case 0x0e:                   // br_table
        count = read_LEB(bytes, pos, 32); // target count
        for (uint32_t i = 0; i < count; i++) {
            read_LEB(bytes, pos, 32);
        }
        read_LEB(bytes, pos, 32);   // default target
        break;
    default:                     // no immediates
        break;
    }
}

void find_blocks(Module *m) {
    Block *function;
    Block *block;
    Block *blockstack[BLOCKSTACK_SIZE];
    int top = -1;
    uint8_t opcode = 0x00;

    trace("  find_blocks: function_count: %d\n", m->function_count);
    for (uint32_t f = m->import_count; f < m->function_count; f++) {
        function = &m->functions[f];
        trace("    fidx: 0x%x, start: 0x%x, end: 0x%x\n",
              f, function->start_addr, function->end_addr);
        uint32_t pos = function->start_addr;

        while (pos <= function->end_addr) {
            opcode = m->bytes[pos];
            switch (opcode) {
            case 0x02:               // block
            case 0x03:               // loop
            case 0x04:               // if
                block = acalloc(1, sizeof(Block), "Block");
                block->block_type = opcode;
                block->type = get_block_type(m->bytes[pos + 1]);
                block->start_addr = pos;
                blockstack[++top] = block;
                m->block_lookup[pos] = block;
                break;
            case 0x05:               // else
                ASSERT(blockstack[top]->block_type == 0x04, "else not matched with if")
                blockstack[top]->else_addr = pos + 1;
                break;
            case 0x0b:               // end
                if (pos == function->end_addr) {
                    break;
                }
                ASSERT(top >= 0, "blockstack underflow");
                block = blockstack[top--];
                block->end_addr = pos;
                if (block->block_type == 0x03) {
                    // loop: label after start
                    block->br_addr = block->start_addr + 2;
                } else {
                    // block, if: label at end
                    block->br_addr = pos;
                }
                trace("      block start: 0x%x, end: 0x%x,"
                      " br_addr: 0x%x, else_addr: 0x%x\n",
                      block->start_addr, block->end_addr, block->br_addr, block->else_addr);
                break;
            }
            skip_immediates(m->bytes, &pos);
        }

        ASSERT(top == -1, "Function ended in middle of block\n")
        ASSERT(opcode == 0x0b, "Function block did not end with 0xb\n")
    }
}

//
// Stack machine (byte code related functions)
//

void push_block(Module *m, Block *block, int sp) {
    m->csp += 1;
    m->callstack[m->csp].block = block;
    m->callstack[m->csp].sp = sp;
    m->callstack[m->csp].fp = m->fp;
    m->callstack[m->csp].ra = m->pc;
}

Block *pop_block(Module *m) {
    Frame *frame = &m->callstack[m->csp--];
    Type *t = frame->block->type;
    // TODO: validate return value if there is one

    m->fp = frame->fp;            // Restore frame pointer

    // Validate the return value
    if (t->result_count == 1) {
        if (m->stack[m->sp].value_type != t->results[0]) {
            snprintf(m->exception, EXCEPTION_SIZE, "call signature mismatch");
            return NULL;
        }
    }
    // Restore stack pointer
    if (t->result_count == 1) {
        // Save top value as result
        if (frame->sp < m->sp) {
            m->stack[frame->sp + 1] = m->stack[m->sp];
            m->sp = frame->sp + 1;
        }
    } else {
        if (frame->sp < m->sp) {
            m->sp = frame->sp;
        }
    }

    if (frame->block->block_type == 0x00) {
        // Function, set pc to return address
        m->pc = frame->ra;
    }

    return frame->block;
}

void import_call(Module *m, uint32_t fidx) {
    Block *func = &m->functions[fidx];
    //Type *type = func->type;

    debug("Import call function %d (%s.%s)\n", fidx, func->import_module,
          func->import_field);
    void (*f)(Module *);

    f = (void (*)(Module *)) func->func_ptr;
    f(m);
}

// Setup a function
// Push params and locals on the stack and save a call frame on the call stack
// Sets new pc value for the start of the function
void setup_call(Module *m, uint32_t fidx) {
    Block *func = &m->functions[fidx];
    Type *type = func->type;

    // Push current frame on the call stack
    push_block(m, func, m->sp - type->param_count);

#ifdef TRACE
    trace("  >> fn0x%x(%d) %s(", fidx, fidx, func->export_name ? func->export_name : "");
    for (int p = type->param_count - 1; p >= 0; p--) {
        trace("%s%s", value_repr(&m->stack[m->sp - p]), p ? " " : "");
    }
    trace("), %d locals, %d results\n", func->local_count, type->result_count);
#endif

    // Push locals (dropping extras)
    m->fp = m->sp - type->param_count + 1;
    // TODO: validate arguments vs formal params

    // Push function locals
    for (uint32_t lidx = 0; lidx < func->local_count; lidx++) {
        m->sp += 1;
        m->stack[m->sp].value_type = func->locals[lidx];
        m->stack[m->sp].value.uint64 = 0; // Initialize whole union to 0
    }

    // Set program counter to start of function
    m->pc = func->start_addr;
    return;
}

bool interpret(Module *m) {
    uint8_t *bytes = m->bytes;
    StackValue *stack = m->stack;

    uint32_t cur_pc;
    //Block *block;
    uint32_t arg, val, fidx, tidx __attribute__((__unused__)), cond, depth, count;
    uint32_t flags, offset, addr;
    uint8_t *maddr, *mem_end;
    uint32_t *depths;
    uint8_t opcode;
    uint32_t a, b, c;             // I32 math
    uint64_t d, e, f;             // I64 math
    float g, h, i;                // F32 math
    double j, k, l;               // F64 math
    bool overflow = false;
    int32_t ocsp = m->csp - 1;

    while (m->pc < m->byte_count) {
        opcode = bytes[m->pc];
        cur_pc = m->pc;
        m->pc += 1;

        if (m->terminated) {
            return false;
        }

        m->gas -= 1;
        if (m->gas <= 0) {
            snprintf(m->exception, EXCEPTION_SIZE, "%s", "insufficient_gas");
            return false;
        }

        if (DEBUG) {
            dump_stacks(m);
        }
        trace("    0x%x <0x%x/%s>\n", cur_pc, opcode, OPERATOR_INFO[opcode]);

        switch (opcode) {

        //
        // Control flow operators
        //
        case 0x00:                 // unreachable
            snprintf(m->exception, EXCEPTION_SIZE, "%s", "unreachable");
            return false;
        case 0x01:                 // nop
            continue;
        case 0x02:                 // block
            read_LEB(bytes, &m->pc, 32);  // ignore block type
            push_block(m, m->block_lookup[cur_pc], m->sp);
            continue;
        case 0x03:                 // loop
            read_LEB(bytes, &m->pc, 32);  // ignore block type
            push_block(m, m->block_lookup[cur_pc], m->sp);
            continue;
        case 0x04:                 // if
            read_LEB(bytes, &m->pc, 32);  // ignore block type
            Block *block = m->block_lookup[cur_pc];

            push_block(m, block, m->sp);

            cond = stack[m->sp--].value.uint32;
            if (cond == 0) {          // if false (I32)
                // branch to else block or after end of if
                if (block->else_addr == 0) {
                    // no else block, pop if block and skip end
                    m->csp -= 1;
                    m->pc = block->br_addr + 1;
                } else {
                    m->pc = block->else_addr;
                }
            }
            // if true, keep going
            if (TRACE) {
                trace("      - cond: 0x%x jump to 0x%x, block: %s\n",
                      cond, m->pc, block_repr(block));
            }
            continue;
        case 0x05:                 // else
            block = m->callstack[m->csp].block;
            m->pc = block->br_addr;
            if (TRACE) {
                trace("      - of %s jump to 0x%x\n", block_repr(block), m->pc);
            }
            continue;
        case 0x0b:                 // end
            block = pop_block(m);
            if (block == NULL) {
                return false;           // an exception (set by pop_block)
            }
            if (TRACE) {
                trace("      - of %s\n", block_repr(block));
            }
            if (block->block_type == 0x00) {  // Function
                trace("  << fn0x%x(%d) %s = %s\n",
                      block->fidx, block->fidx,
                      block->export_name ? block->export_name : "",
                      block->type->result_count > 0 ? value_repr(&m->stack[m->sp]) : "_");
                if (m->csp == ocsp) {
                    trace("Finish interpreting\n");
                    // Return to top-level
                    return true;
                } else {
                    trace("Keep goint interpreting: csp %d ocsp %d\n", m->csp, ocsp);
                    // Keep going at return address
                }
            } else if (block->block_type == 0x01) { // init_expr
                return true;
            } else {                  // Block
                // End of block/loop/if, keep going
            }
            continue;
        case 0x0c:                 // br
            depth = read_LEB(bytes, &m->pc, 32);
            m->csp -= depth;
            // set to end for pop_block
            m->pc = m->callstack[m->csp].block->br_addr;
            if (TRACE) {
                // TODO: wtf?
                /*debug("      - to: 0x%x\n", &m->pc);*/
                trace("      - to: 0x%x\n", m->pc);
            }
            continue;
        case 0x0d:                 // br_if
            depth = read_LEB(bytes, &m->pc, 32);

            cond = stack[m->sp--].value.uint32;
            if (cond) {               // if true
                m->csp -= depth;
                // set to end for pop_block
                m->pc = m->callstack[m->csp].block->br_addr;
            }
            trace("      - depth: 0x%x, cond: 0x%x, to: 0x%x\n", depth, cond, m->pc);
            continue;
        case 0x0e:                 // br_table
            count = read_LEB(bytes, &m->pc, 32);
            depths = acalloc(count, sizeof(uint32_t), "uint32_t");
            for (uint32_t i = 0; i < count; i++) {
                depths[i] = read_LEB(bytes, &m->pc, 32);
            }
            depth = read_LEB(bytes, &m->pc, 32);

            int32_t didx = stack[m->sp--].value.uint32;

            if (didx >= 0 && didx < (int32_t) count) {
                depth = depths[didx];
            }
            free(depths);

            m->csp -= depth;
            // set to end for pop_block
            m->pc = m->callstack[m->csp].block->br_addr;
            trace("      - count: %d, didx: %d, to: 0x%x\n", count,
                  didx, m->pc);
            continue;
        case 0x0f:                 // return
            while (m->csp >= 0 && m->callstack[m->csp].block->block_type != 0x00) {
                m->csp--;
            }
            // Set the program count to the end of the function
            // The actual pop_block and return is handled by the end opcode.
            m->pc = m->callstack[0].block->end_addr;
            trace("      - to: 0x%x\n", m->pc);
            continue;

        //
        // Call operators
        //
        case 0x10:                 // call
            fidx = read_LEB(bytes, &m->pc, 32);

            if (fidx < m->import_count) {
                import_call(m, fidx);   // import/thunk call
            } else {
                setup_call(m, fidx);    // regular function call
                trace("      - calling function fidx: %d at: 0x%x\n", fidx, m->pc);
            }
            continue;
        case 0x11:                 // call_indirect
            tidx = read_LEB(bytes, &m->pc, 32); // TODO: use tidx?
            read_LEB(bytes, &m->pc, 1); // reserved immediate
            val = stack[m->sp--].value.uint32;
            if (m->options.mangle_table_index) {
                // val is the table address + the index (not sized for the
                // pointer size) so get the actual (sized) index
                trace("      - entries: %p, original val: 0x%x, new val: 0x%x\n",
                      (void *) m->table.entries, val, (uint32_t) m->table.entries - val);
                val = val - (unsigned long) m->table.entries;
            }
            if (val >= m->table.maximum) {
                snprintf(m->exception, EXCEPTION_SIZE, "undefined element 0x%x", val);
                return false;
            }

            fidx = m->table.entries[val];
            trace("       - call_indirect tidx: %d, val: 0x%x, fidx: 0x%x\n",
                  tidx, val, fidx);

            if (fidx < m->import_count) {
                import_call(m, fidx);   // import/thunk call
            } else {
                setup_call(m, fidx);    // regular function call

                Block *func = &m->functions[fidx];
                Type *ftype = func->type;

                // Validate signatures match
                if ((int32_t) (ftype->param_count + func->local_count) != m->sp - m->fp + 1) {
                    snprintf(m->exception, EXCEPTION_SIZE, "indirect call signature mismatch");
                    return false;
                }
                for (uint32_t f = 0; f < ftype->param_count; f++) {
                    if (ftype->params[f] != m->stack[m->fp + f].value_type) {
                        snprintf(m->exception, EXCEPTION_SIZE, "indirect call signature mismatch");
                        return false;
                    }
                }

                trace("      - tidx: %d, table idx: %d, "
                      "calling function fidx: %d at: 0x%x\n", tidx, val, fidx, m->pc);
            }
            continue;

        //
        // Parametric operators
        //
        case 0x1a:                 // drop
            m->sp--;
            continue;
        case 0x1b:                 // select
            cond = stack[m->sp--].value.uint32;
            m->sp--;
            if (!cond) {              // use a instead of b
                stack[m->sp] = stack[m->sp + 1];
            }
            continue;

        //
        // Variable access
        //
        case 0x20:                 // get_local
            arg = read_LEB(bytes, &m->pc, 32);
            trace("      - arg: 0x%x, got %s\n", arg,
                  value_repr(&stack[m->fp + arg]));
            stack[++m->sp] = stack[m->fp + arg];
            continue;
        case 0x21:                 // set_local
            arg = read_LEB(bytes, &m->pc, 32);
            stack[m->fp + arg] = stack[m->sp--];
            trace("      - arg: 0x%x, to %s\n", arg, value_repr(&stack[m->sp]));
            continue;
        case 0x22:                 // tee_local
            arg = read_LEB(bytes, &m->pc, 32);
            stack[m->fp + arg] = stack[m->sp];
            trace("      - arg: 0x%x, to %s\n", arg, value_repr(&stack[m->sp]));
            continue;
        case 0x23:                 // get_global
            arg = read_LEB(bytes, &m->pc, 32);
            trace("      - arg: 0x%x, got %s\n", arg,
                  value_repr(&m->globals[arg]));
            stack[++m->sp] = m->globals[arg];
            continue;
        case 0x24:                 // set_global
            arg = read_LEB(bytes, &m->pc, 32);
            m->globals[arg] = stack[m->sp--];
            trace("      - arg: 0x%x, to %s\n", arg, value_repr(&m->globals[arg]));
            continue;

        //
        // Memory-related operators
        //
        case 0x3f:                 // current_memory
            read_LEB(bytes, &m->pc, 32);  // ignore reserved
            stack[++m->sp].value_type = I32;
            stack[m->sp].value.uint32 = m->memory.pages;
            continue;
        case 0x40:                 // grow_memory
            read_LEB(bytes, &m->pc, 32);  // ignore reserved
            uint32_t prev_pages = m->memory.pages;
            uint32_t delta = stack[m->sp].value.uint32;

            stack[m->sp].value.uint32 = prev_pages;
            if (delta == 0) {
                continue;               // No change
            } else if (delta + prev_pages > m->memory.maximum) {
                stack[m->sp].value.uint32 = -1;
                continue;
            }
            m->memory.pages += delta;
            m->memory.bytes = arecalloc(m->memory.bytes,
                                        prev_pages * pow(2, 16),
                                        m->memory.pages * pow(2, 16),
                                        sizeof(uint32_t), "Module->memory.bytes");
            debug(
                "GROW MEMORY, now  pages = %u\n", m->memory.pages);
            continue;

        // Memory load operators
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
            flags = read_LEB(bytes, &m->pc, 32);
            offset = read_LEB(bytes, &m->pc, 32);
            addr = stack[m->sp--].value.uint32;
            if (flags != 2 && TRACE) {
                warn("      - unaligned load - flags: 0x%x,"
                     " offset: 0x%x, addr: 0x%x\n", flags, offset, addr);
            }
            if (offset + addr < addr) {
                overflow = true;
            }
            maddr = m->memory.bytes + offset + addr;
            if (maddr < m->memory.bytes) {
                overflow = true;
            }
            mem_end = m->memory.bytes + m->memory.pages * (uint32_t) pow(2, 16);
            if (maddr + LOAD_SIZE[opcode - 0x28] > mem_end) {
                overflow = true;
            }
            trace("      - addr: 0x%x, offset: 0x%x, maddr: %p, mem_end: %p\n",
                  addr, offset, (void *) maddr, (void *) mem_end);
            if (overflow) {
                warn("memory start: %p, memory end: %p, maddr: %p\n",
                     (void *) m->memory.bytes, (void *) mem_end, (void *) maddr);
                snprintf(m->exception, EXCEPTION_SIZE, "out of bounds memory access");
                return false;
            }
            stack[++m->sp].value.uint64 = 0;  // initialize to 0
            switch (opcode) {
            case 0x28:
                memcpy(&stack[m->sp].value, maddr, 4);
                stack[m->sp].value_type = I32;
                break;                  // i32.load
            case 0x29:
                memcpy(&stack[m->sp].value, maddr, 8);
                stack[m->sp].value_type = I64;
                break;                  // i64.load
            case 0x2a:
                memcpy(&stack[m->sp].value, maddr, 4);
                stack[m->sp].value_type = F32;
                break;                  // f32.load
            case 0x2b:
                memcpy(&stack[m->sp].value, maddr, 8);
                stack[m->sp].value_type = F64;
                break;                  // f64.load
            case 0x2c:
                memcpy(&stack[m->sp].value, maddr, 1);
                sext_8_32(&stack[m->sp].value.uint32);
                stack[m->sp].value_type = I32;
                break;                  // i32.load8_s
            case 0x2d:
                memcpy(&stack[m->sp].value, maddr, 1);
                stack[m->sp].value_type = I32;
                break;                  // i32.load8_u
            case 0x2e:
                memcpy(&stack[m->sp].value, maddr, 2);
                sext_16_32(&stack[m->sp].value.uint32);
                stack[m->sp].value_type = I32;
                break;                  // i32.load16_s
            case 0x2f:
                memcpy(&stack[m->sp].value, maddr, 2);
                stack[m->sp].value_type = I32;
                break;                  // i32.load16_u
            case 0x30:
                memcpy(&stack[m->sp].value, maddr, 1);
                sext_8_64(&stack[m->sp].value.uint64);
                stack[m->sp].value_type = I64;
                break;                  // i64.load8_s
            case 0x31:
                memcpy(&stack[m->sp].value, maddr, 1);
                stack[m->sp].value_type = I64;
                break;                  // i64.load8_u
            case 0x32:
                memcpy(&stack[m->sp].value, maddr, 2);
                sext_16_64(&stack[m->sp].value.uint64);
                stack[m->sp].value_type = I64;
                break;                  // i64.load16_s
            case 0x33:
                memcpy(&stack[m->sp].value, maddr, 2);
                stack[m->sp].value_type = I64;
                break;                  // i64.load16_u
            case 0x34:
                memcpy(&stack[m->sp].value, maddr, 4);
                sext_32_64(&stack[m->sp].value.uint64);
                stack[m->sp].value_type = I64;
                break;                  // i64.load32_s
            case 0x35:
                memcpy(&stack[m->sp].value, maddr, 4);
                stack[m->sp].value_type = I64;
                break;                  // i64.load32_u
            }
            continue;

        // Memory store operators
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
            flags = read_LEB(bytes, &m->pc, 32);
            offset = read_LEB(bytes, &m->pc, 32);
            StackValue *sval = &stack[m->sp--];

            addr = stack[m->sp--].value.uint32;
            if (flags != 2 && TRACE) {
                warn("      - unaligned store - flags: 0x%x,"
                     " offset: 0x%x, addr: 0x%x, val: %s\n",
                     flags, offset, addr, value_repr(sval));
            }
            if (offset + addr < addr) {
                overflow = true;
            }
            maddr = m->memory.bytes + offset + addr;
            if (maddr < m->memory.bytes) {
                overflow = true;
            }
            mem_end = m->memory.bytes + m->memory.pages * (uint32_t) pow(2, 16);
            if (maddr + LOAD_SIZE[opcode - 0x28] > mem_end) {
                overflow = true;
            }
            trace("      - addr: 0x%x, offset: 0x%x, maddr: %p, mem_end: %p, value: %s\n",
                  addr, offset, (void *) maddr, (void *) mem_end, value_repr(sval));
            if (overflow) {
                warn("memory start: %p, memory end: %p, maddr: %p\n",
                     (void *) m->memory.bytes, (void *) mem_end, (void *) maddr);
                snprintf(m->exception, EXCEPTION_SIZE, "out of bounds memory access");
                return false;
            }
            switch (opcode) {
            case 0x36:
                memcpy(maddr, &sval->value.uint32, 4);
                break;                  // i32.store
            case 0x37:
                memcpy(maddr, &sval->value.uint64, 8);
                break;                  // i64.store
            case 0x38:
                memcpy(maddr, &sval->value.f32, 4);
                break;                  // f32.store
            case 0x39:
                memcpy(maddr, &sval->value.f64, 8);
                break;                  // f64.store
            case 0x3a:
                memcpy(maddr, &sval->value.uint32, 1);
                break;                  // i32.store8
            case 0x3b:
                memcpy(maddr, &sval->value.uint32, 2);
                break;                  // i32.store16
            case 0x3c:
                memcpy(maddr, &sval->value.uint64, 1);
                break;                  // i64.store8
            case 0x3d:
                memcpy(maddr, &sval->value.uint64, 2);
                break;                  // i64.store16
            case 0x3e:
                memcpy(maddr, &sval->value.uint64, 4);
                break;                  // i64.store32
            }
            continue;

        //
        // Constants
        //
        case 0x41:                 // i32.const
            stack[++m->sp].value_type = I32;
            stack[m->sp].value.uint32 = read_LEB_signed(bytes, &m->pc, 32);
            continue;
        case 0x42:                 // i64.const
            stack[++m->sp].value_type = I64;
            stack[m->sp].value.int64 = read_LEB_signed(bytes, &m->pc, 64);
            continue;
        case 0x43:                 // f32.const
            stack[++m->sp].value_type = F32;
            memcpy(&stack[m->sp].value.uint32, bytes + m->pc, 4);
            m->pc += 4;
            // stack[m->sp].value.uint32 = read_LEB_signed(bytes, pm->c, 32);
            continue;
        case 0x44:                 // f64.const
            stack[++m->sp].value_type = F64;
            memcpy(&stack[m->sp].value.uint64, bytes + m->pc, 8);
            m->pc += 8;
            // stack[m->sp].value.uint64 = read_LEB_signed(bytes, m->pc, 64);
            continue;

        //
        // Comparison operators
        //

        // unary
        case 0x45:                 // i32.eqz
            stack[m->sp].value.uint32 = stack[m->sp].value.uint32 == 0;
            continue;
        case 0x50:                 // i64.eqz
            stack[m->sp].value_type = I32;
            stack[m->sp].value.uint32 = stack[m->sp].value.uint64 == 0;
            continue;

        // i32 binary
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
            a = stack[m->sp - 1].value.uint32;
            b = stack[m->sp].value.uint32;
            m->sp -= 1;
            switch (opcode) {
            case 0x46:
                c = a == b;
                break;                  // i32.eq
            case 0x47:
                c = a != b;
                break;                  // i32.ne
            case 0x48:
                c = (int32_t) a < (int32_t) b;
                break;                  // i32.lt_s
            case 0x49:
                c = a < b;
                break;                  // i32.lt_u
            case 0x4a:
                c = (int32_t) a > (int32_t) b;
                break;                  // i32.gt_s
            case 0x4b:
                c = a > b;
                break;                  // i32.gt_u
            case 0x4c:
                c = (int32_t) a <= (int32_t) b;
                break;                  // i32.le_s
            case 0x4d:
                c = a <= b;
                break;                  // i32.le_u
            case 0x4e:
                c = (int32_t) a >= (int32_t) b;
                break;                  // i32.ge_s
            case 0x4f:
                c = a >= b;
                break;                  // i32.ge_u
            }
            stack[m->sp].value_type = I32;
            stack[m->sp].value.uint32 = c;
            continue;
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
            d = stack[m->sp - 1].value.uint64;
            e = stack[m->sp].value.uint64;
            m->sp -= 1;
            switch (opcode) {
            case 0x51:
                c = d == e;
                break;                  // i64.eq
            case 0x52:
                c = d != e;
                break;                  // i64.ne
            case 0x53:
                c = (int64_t) d < (int64_t) e;
                break;                  // i64.lt_s
            case 0x54:
                c = d < e;
                break;                  // i64.lt_u
            case 0x55:
                c = (int64_t) d > (int64_t) e;
                break;                  // i64.gt_s
            case 0x56:
                c = d > e;
                break;                  // i64.gt_u
            case 0x57:
                c = (int64_t) d <= (int64_t) e;
                break;                  // i64.le_s
            case 0x58:
                c = d <= e;
                break;                  // i64.le_u
            case 0x59:
                c = (int64_t) d >= (int64_t) e;
                break;                  // i64.ge_s
            case 0x5a:
                c = d >= e;
                break;                  // i64.ge_u
            }
            stack[m->sp].value_type = I32;
            stack[m->sp].value.uint32 = c;
            continue;
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
        case 0x60:
            g = stack[m->sp - 1].value.f32;
            h = stack[m->sp].value.f32;
            m->sp -= 1;
            switch (opcode) {
            case 0x5b:
                c = g == h;
                break;                  // f32.eq
            case 0x5c:
                c = g != h;
                break;                  // f32.ne
            case 0x5d:
                c = g < h;
                break;                  // f32.lt
            case 0x5e:
                c = g > h;
                break;                  // f32.gt
            case 0x5f:
                c = g <= h;
                break;                  // f32.le
            case 0x60:
                c = g >= h;
                break;                  // f32.ge
            }
            stack[m->sp].value_type = I32;
            stack[m->sp].value.uint32 = c;
            continue;
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
            j = stack[m->sp - 1].value.f64;
            k = stack[m->sp].value.f64;
            m->sp -= 1;
            switch (opcode) {
            case 0x61:
                c = j == k;
                break;                  // f64.eq
            case 0x62:
                c = j != k;
                break;                  // f64.ne
            case 0x63:
                c = j < k;
                break;                  // f64.lt
            case 0x64:
                c = j > k;
                break;                  // f64.gt
            case 0x65:
                c = j <= k;
                break;                  // f64.le
            case 0x66:
                c = j >= k;
                break;                  // f64.ge
            }
            stack[m->sp].value_type = I32;
            stack[m->sp].value.uint32 = c;
            continue;

        //
        // Numeric operators
        //

        // unary i32
        case 0x67:
        case 0x68:
        case 0x69:
            a = stack[m->sp].value.uint32;
            switch (opcode) {
            case 0x67:
                c = a == 0 ? 32 : __builtin_clz(a);
                break;                  // i32.clz
            case 0x68:
                c = a == 0 ? 32 : __builtin_ctz(a);
                break;                  // i32.ctz
            case 0x69:
                c = __builtin_popcount(a);
                break;                  // i32.popcnt
            }
            stack[m->sp].value.uint32 = c;
            continue;

        // unary i64
        case 0x79:
        case 0x7a:
        case 0x7b:
            d = stack[m->sp].value.uint64;
            switch (opcode) {
            case 0x79:
                f = d == 0 ? 64 : __builtin_clzll(d);
                break;                  // i64.clz
            case 0x7a:
                f = d == 0 ? 64 : __builtin_ctzll(d);
                break;                  // i64.ctz
            case 0x7b:
                f = __builtin_popcountll(d);
                break;                  // i64.popcnt
            }
            stack[m->sp].value.uint64 = f;
            continue;

        // unary f32
        case 0x8b:
            stack[m->sp].value.f32 = fabs(stack[m->sp].value.f32);
            break;                    // f32.abs
        case 0x8c:
            stack[m->sp].value.f32 = -stack[m->sp].value.f32;
            break;                    // f32.neg
        case 0x8d:
            stack[m->sp].value.f32 = ceil(stack[m->sp].value.f32);
            break;                    // f32.ceil
        case 0x8e:
            stack[m->sp].value.f32 = floor(stack[m->sp].value.f32);
            break;                    // f32.floor
        case 0x8f:
            stack[m->sp].value.f32 = trunc(stack[m->sp].value.f32);
            break;                    // f32.trunc
        case 0x90:
            stack[m->sp].value.f32 = rint(stack[m->sp].value.f32);
            break;                    // f32.nearest
        case 0x91:
            stack[m->sp].value.f32 = sqrt(stack[m->sp].value.f32);
            break;                    // f32.sqrt

        // unary f64
        case 0x99:
            stack[m->sp].value.f64 = fabs(stack[m->sp].value.f64);
            break;                    // f64.abs
        case 0x9a:
            stack[m->sp].value.f64 = -stack[m->sp].value.f64;
            break;                    // f64.neg
        case 0x9b:
            stack[m->sp].value.f64 = ceil(stack[m->sp].value.f64);
            break;                    // f64.ceil
        case 0x9c:
            stack[m->sp].value.f64 = floor(stack[m->sp].value.f64);
            break;                    // f64.floor
        case 0x9d:
            stack[m->sp].value.f64 = trunc(stack[m->sp].value.f64);
            break;                    // f64.trunc
        case 0x9e:
            stack[m->sp].value.f64 = rint(stack[m->sp].value.f64);
            break;                    // f64.nearest
        case 0x9f:
            stack[m->sp].value.f64 = sqrt(stack[m->sp].value.f64);
            break;                    // f64.sqrt

        // i32 binary
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
            a = stack[m->sp - 1].value.uint32;
            b = stack[m->sp].value.uint32;
            m->sp -= 1;
            if (opcode >= 0x6d && opcode <= 0x70 && b == 0) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer divide by zero");
                return false;
            }
            switch (opcode) {
            // case 0x6a: o = __builtin_add_overflow(a, b, &c); break; // i32.add
            // case 0x6b: o = __builtin_sub_overflow(a, b, &c); break; // i32.sub
            case 0x6a:
                c = a + b;
                break;                  // i32.add
            case 0x6b:
                c = a - b;
                break;                  // i32.sub
            case 0x6c:
                c = a * b;
                break;                  // i32.mul
            case 0x6d:
                if (a == 0x80000000 && b == 0xFFFFFFFF) {
                    snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                    return false;
                }
                c = (int32_t) a / (int32_t) b;
                break;                  // i32.div_s
            case 0x6e:
                c = a / b;
                break;                  // i32.div_u
            case 0x6f:
                if (a == 0x80000000 && b == 0xFFFFFFFF) {
                    c = 0;
                } else {
                    c = (int32_t) a % (int32_t) b;
                };
                break;                  // i32.rem_s
            case 0x70:
                c = a % b;
                break;                  // i32.rem_u
            case 0x71:
                c = a & b;
                break;                  // i32.and
            case 0x72:
                c = a | b;
                break;                  // i32.or
            case 0x73:
                c = a ^ b;
                break;                  // i32.xor
            case 0x74:
                c = a << b;
                break;                  // i32.shl
            case 0x75:
                c = (int32_t) a >> b;
                break;                  // i32.shr_s
            case 0x76:
                c = a >> b;
                break;                  // i32.shr_u
            case 0x77:
                c = rotl32(a, b);
                break;                  // i32.rotl
            case 0x78:
                c = rotr32(a, b);
                break;                  // i32.rotr
            }
            // if (o == 1) {
            // snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
            // return false;
            // }
            stack[m->sp].value.uint32 = c;
            continue;

        // i64 binary
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f:
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
            d = stack[m->sp - 1].value.uint64;
            e = stack[m->sp].value.uint64;
            m->sp -= 1;
            if (opcode >= 0x7f && opcode <= 0x82 && e == 0) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer divide by zero");
                return false;
            }
            switch (opcode) {
            case 0x7c:
                f = d + e;
                break;                  // i64.add
            case 0x7d:
                f = d - e;
                break;                  // i64.sub
            case 0x7e:
                f = d * e;
                break;                  // i64.mul
            case 0x7f:
                if (d == 0x8000000000000000 && e == 0xFFFFFFFFFFFFFFFF) {
                    snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                    return false;
                }
                f = (int64_t) d / (int64_t) e;
                break;                  // i64.div_s
            case 0x80:
                f = d / e;
                break;                  // i64.div_u
            case 0x81:
                if (d == 0x8000000000000000 && e == 0xFFFFFFFFFFFFFFFF) {
                    f = 0;
                } else {
                    f = (int64_t) d % (int64_t) e;
                }
                break;                  // i64.rem_s
            case 0x82:
                f = d % e;
                break;                  // i64.rem_u
            case 0x83:
                f = d & e;
                break;                  // i64.and
            case 0x84:
                f = d | e;
                break;                  // i64.or
            case 0x85:
                f = d ^ e;
                break;                  // i64.xor
            case 0x86:
                f = d << e;
                break;                  // i64.shl
            case 0x87:
                f = ((int64_t) d) >> e;
                break;                  // i64.shr_s
            case 0x88:
                f = d >> e;
                break;                  // i64.shr_u
            case 0x89:
                f = rotl64(d, e);
                break;                  // i64.rotl
            case 0x8a:
                f = rotr64(d, e);
                break;                  // i64.rotr
            }
            stack[m->sp].value.uint64 = f;
            continue;

        // f32 binary
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
            g = stack[m->sp - 1].value.f32;
            h = stack[m->sp].value.f32;
            m->sp -= 1;
            switch (opcode) {
            case 0x92:
                i = g + h;
                break;                  // f32.add
            case 0x93:
                i = g - h;
                break;                  // f32.sub
            case 0x94:
                i = g * h;
                break;                  // f32.mul
            case 0x95:
                i = g / h;
                break;                  // f32.div
            case 0x96:
                i = wa_fmin(g, h);
                break;                  // f32.min
            case 0x97:
                i = wa_fmax(g, h);
                break;                  // f32.max
            case 0x98:
                i = signbit(h) ? -fabs(g) : fabs(g);
                break;                  // f32.copysign
            }
            stack[m->sp].value.f32 = i;
            continue;

        // f64 binary
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
            j = stack[m->sp - 1].value.f64;
            k = stack[m->sp].value.f64;
            m->sp -= 1;
            switch (opcode) {
            case 0xa0:
                l = j + k;
                break;                  // f64.add
            case 0xa1:
                l = j - k;
                break;                  // f64.sub
            case 0xa2:
                l = j * k;
                break;                  // f64.mul
            case 0xa3:
                l = j / k;
                break;                  // f64.div
            case 0xa4:
                l = wa_fmin(j, k);
                break;                  // f64.min
            case 0xa5:
                l = wa_fmax(j, k);
                break;                  // f64.max
            case 0xa6:
                l = signbit(k) ? -fabs(j) : fabs(j);
                break;                  // f64.copysign
            }
            stack[m->sp].value.f64 = l;
            continue;

        // conversion operations
        // case 0xa7 ... 0xbb:
        case 0xa7:
            stack[m->sp].value.uint64 &= 0x00000000ffffffff;
            stack[m->sp].value_type = I32;
            break;                    // i32.wrap/i64
        case 0xa8:
            if (isnan(stack[m->sp].value.f32)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f32 >= (float)INT32_MAX ||
                       stack[m->sp].value.f32 < (float)INT32_MIN) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.int32 = stack[m->sp].value.f32;
            stack[m->sp].value_type = I32;
            break;                    // i32.trunc_s/f32
        case 0xa9:
            if (isnan(stack[m->sp].value.f32)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f32 >= (float)UINT32_MAX || stack[m->sp].value.f32 <= -1) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.uint32 = stack[m->sp].value.f32;
            stack[m->sp].value_type = I32;
            break;                    // i32.trunc_u/f32
        case 0xaa:
            if (isnan(stack[m->sp].value.f64)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f64 > (double)INT32_MAX || stack[m->sp].value.f64 < INT32_MIN) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.int32 = stack[m->sp].value.f64;
            stack[m->sp].value_type = I32;
            break;                    // i32.trunc_s/f64
        case 0xab:
            if (isnan(stack[m->sp].value.f64)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f64 > UINT32_MAX || stack[m->sp].value.f64 <= -1) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.uint32 = stack[m->sp].value.f64;
            stack[m->sp].value_type = I32;
            break;                    // i32.trunc_u/f64
        case 0xac:
            stack[m->sp].value.uint64 = stack[m->sp].value.uint32;
            sext_32_64(&stack[m->sp].value.uint64);
            stack[m->sp].value_type = I64;
            break;                    // i64.extend_s/i32
        case 0xad:
            stack[m->sp].value.uint64 = stack[m->sp].value.uint32;
            stack[m->sp].value_type = I64;
            break;                    // i64.extend_u/i32
        case 0xae:
            if (isnan(stack[m->sp].value.f32)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f32 >= (float)INT64_MAX ||
                       stack[m->sp].value.f32 < (float)INT64_MIN) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.int64 = stack[m->sp].value.f32;
            stack[m->sp].value_type = I64;
            break;                    // i64.trunc_s/f32
        case 0xaf:
            if (isnan(stack[m->sp].value.f32)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f32 >= (float)UINT64_MAX || stack[m->sp].value.f32 <= -1) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.uint64 = stack[m->sp].value.f32;
            stack[m->sp].value_type = I64;
            break;                    // i64.trunc_u/f32
        case 0xb0:
            if (isnan(stack[m->sp].value.f64)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f64 >= (double)INT64_MAX ||
                       stack[m->sp].value.f64 < (double)INT64_MIN) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.int64 = stack[m->sp].value.f64;
            stack[m->sp].value_type = I64;
            break;                    // i64.trunc_s/f64
        case 0xb1:
            if (isnan(stack[m->sp].value.f64)) {
                snprintf(m->exception, EXCEPTION_SIZE, "invalid conversion to integer");
                return false;
            } else if (stack[m->sp].value.f64 >= (double)UINT64_MAX || stack[m->sp].value.f64 <= -1) {
                snprintf(m->exception, EXCEPTION_SIZE, "integer overflow");
                return false;
            }
            stack[m->sp].value.uint64 = stack[m->sp].value.f64;
            stack[m->sp].value_type = I64;
            break;                    // i64.trunc_u/f64
        case 0xb2:
            stack[m->sp].value.f32 = stack[m->sp].value.int32;
            stack[m->sp].value_type = F32;
            break;                    // f32.convert_s/i32
        case 0xb3:
            stack[m->sp].value.f32 = stack[m->sp].value.uint32;
            stack[m->sp].value_type = F32;
            break;                    // f32.convert_u/i32
        case 0xb4:
            stack[m->sp].value.f32 = stack[m->sp].value.int64;
            stack[m->sp].value_type = F32;
            break;                    // f32.convert_s/i64
        case 0xb5:
            stack[m->sp].value.f32 = stack[m->sp].value.uint64;
            stack[m->sp].value_type = F32;
            break;                    // f32.convert_u/i64
        case 0xb6:
            stack[m->sp].value.f32 = stack[m->sp].value.f64;
            stack[m->sp].value_type = F32;
            break;                    // f32.demote/f64
        case 0xb7:
            stack[m->sp].value.f64 = stack[m->sp].value.int32;
            stack[m->sp].value_type = F64;
            break;                    // f64.convert_s/i32
        case 0xb8:
            stack[m->sp].value.f64 = stack[m->sp].value.uint32;
            stack[m->sp].value_type = F64;
            break;                    // f64.convert_u/i32
        case 0xb9:
            stack[m->sp].value.f64 = stack[m->sp].value.int64;
            stack[m->sp].value_type = F64;
            break;                    // f64.convert_s/i64
        case 0xba:
            stack[m->sp].value.f64 = stack[m->sp].value.uint64;
            stack[m->sp].value_type = F64;
            break;                    // f64.convert_u/i64
        case 0xbb:
            stack[m->sp].value.f64 = stack[m->sp].value.f32;
            stack[m->sp].value_type = F64;
            break;                    // f64.promote/f32

        // reinterpretations
        case 0xbc:
            stack[m->sp].value_type = I32;
            break;                    // i32.reinterpret/f32
        case 0xbd:
            stack[m->sp].value_type = I64;
            break;                    // i64.reinterpret/f64
        case 0xbe:                 // memmove(&stack[m->sp].value.f32,
            // &stack[m->sp].value.uint32, 4);
            stack[m->sp].value_type = F32;
            break;                    // f32.reinterpret/i32
        case 0xbf:
            stack[m->sp].value_type = F64;
            break;                    // f64.reinterpret/i64

        default:
            snprintf(m->exception, EXCEPTION_SIZE, "unrecognized opcode 0x%x", opcode);
            return false;
        }
    }
    return false;
}

void run_init_expr(Module *m, uint8_t type, uint32_t *pc) {
    // Run the init_expr
    Block block = {.block_type = 0x01,
                   .type = get_block_type(type),
                   .start_addr = *pc
                  };
    m->pc = *pc;
    push_block(m, &block, m->sp);
    // WARNING: running code here to get initial value!
    trace("  running init_expr at 0x%x: %s\n", m->pc, block_repr(&block));
    interpret(m);
    *pc = m->pc;

    trace("init_expr = '%s'\n", value_repr(&(m->stack[m->sp])));
    ASSERT(m->stack[m->sp].value_type == type,
           "init_expr type mismatch 0x%x != 0x%x", m->stack[m->sp].value_type, type);
}

//
// Public API
//

uint32_t get_export_fidx(Module *m, char *name) {
    // Find name function index
    for (uint32_t f = 0; f < m->function_count; f++) {
        char *fname = m->functions[f].export_name;

        if (!fname) {
            continue;
        }
        if (strncmp(name, fname, 1024) == 0) {
            return f;
        }
    }
    return -1;
}


Module *load_module_from_file(char *path, Options options) {
    uint32_t mod_len;
    uint8_t *bytes = mmap_file(path, &mod_len);
    Module *mod = load_module(bytes, (size_t) mod_len, options);
    mod->path = path;
    return mod;
}

Module *load_module(uint8_t *bytes, size_t code_len, Options options) {
    uint8_t vt;
    uint32_t pos = 0, word;
    Module *m;

    // Allocate the module
    m = acalloc(1, sizeof(Module), "Module");
    m->path = NULL;
    m->options = options;

    // Empty stacks
    m->sp = -1;
    m->fp = -1;
    m->csp = -1;
    m->gas = 1000;

    // open and mmap the WASM module
    m->byte_count = code_len;
    m->bytes = bytes;
    m->block_lookup = acalloc(m->byte_count, sizeof(Block *), "function->block_lookup");
    m->start_function = 0xFFFFFFFF;

    // Check the module
    pos = 0;
    word = read_uint32(bytes, &pos);
    ASSERT(word == WA_MAGIC, "Wrong module magic 0x%x\n", word);
    word = read_uint32(bytes, &pos);
    ASSERT(word == WA_VERSION, "Wrong module version 0x%x\n", word);

    // Read the sections
    while (pos < code_len) {
        uint32_t id = read_LEB(bytes, &pos, 7);
        uint32_t slen = read_LEB(bytes, &pos, 32);
        int start_pos = pos;

        trace("Reading section %d at 0x%x, length %d\n", id, pos, slen);
        switch (id) {
        case 0:
            trace("Parsing Custom(0) section (length: 0x%x)\n", slen);
            uint32_t end_pos = pos + slen;
            char *name = read_string(bytes, &pos, NULL);

            trace("  Section name '%s'\n", name);
            if (strncmp(name, "dylink", 7) == 0) {
                // https://github.com/WebAssembly/tool-conventions/blob/master/DynamicLinking.md
                // TODO: make use of these
                uint32_t memorysize = read_LEB(bytes, &pos, 32);
                uint32_t tablesize = read_LEB(bytes, &pos, 32);
                (void) memorysize;
                (void) tablesize;
            } else {
                error("Ignoring unknown custom section '%s'\n", name);
            }
            pos = end_pos;
            free(name);
            break;
        case 1:
            trace("Parsing Type(1) section (length: 0x%x)\n", slen);
            m->type_count = read_LEB(bytes, &pos, 32);
            m->types = acalloc(m->type_count, sizeof(Type), "Module->types");

            for (uint32_t c = 0; c < m->type_count; c++) {
                Type *type = &m->types[c];

                type->form = read_LEB(bytes, &pos, 7);
                type->param_count = read_LEB(bytes, &pos, 32);
                type->params = acalloc(type->param_count, sizeof(uint32_t), "type->params");
                for (uint32_t p = 0; p < type->param_count; p++) {
                    type->params[p] = read_LEB(bytes, &pos, 32);
                }
                type->result_count = read_LEB(bytes, &pos, 32);
                ASSERT(type->result_count <= 1, "Wrong number of results %d\n",
                       type->result_count);
                type->results = acalloc(type->result_count, sizeof(uint32_t), "type->results");
                for (uint32_t r = 0; r < type->result_count; r++) {
                    type->results[r] = read_LEB(bytes, &pos, 32);
                }
                trace("  form: 0x%x, params: %d, results: %d\n",
                      type->form, type->param_count, type->result_count);
            }
            break;
        case 2:
            trace("Parsing Import(2) section (length: 0x%x)\n", slen);
            uint32_t import_count = read_LEB(bytes, &pos, 32);

            for (uint32_t gidx = 0; gidx < import_count; gidx++) {
                uint32_t module_len, field_len;
                char *import_module = read_string(bytes, &pos, &module_len);
                char *import_field = read_string(bytes, &pos, &field_len);

                uint32_t external_kind = bytes[pos++];

                trace("  import: %d/%d, external_kind: %d, %s.%s\n",
                      gidx, import_count, external_kind, import_module, import_field);

                uint32_t type_index, fidx;
                uint8_t element_type, content_type, mutability;

                switch (external_kind) {
                case 0x00:             // Function
                    type_index = read_LEB(bytes, &pos, 32);
                    break;
                case 0x01:             // Table
                    parse_table_type(m, &pos);
                    break;
                case 0x02:             // Memory
                    parse_memory_type(m, &pos);
                    break;
                case 0x03:             // Global
                    content_type = read_LEB(bytes, &pos, 7);
                    mutability = read_LEB(bytes, &pos, 1);
                    break;
                }
                // TODO: make use of this
                (void) element_type;
                (void) content_type;
                (void) mutability;

                void *val;
                char *err;

                if (!options.resolvesym(import_module, import_field, &val, &err)) {
                    // TODO: handle error
                    FATAL("Error: %s\n", err);
                }
                // Store in the right place
                switch (external_kind) {
                case 0x00:             // Function
                    fidx = m->function_count;
                    m->import_count += 1;
                    m->function_count += 1;
                    m->functions = arecalloc(m->functions,
                                             fidx, m->import_count,
                                             sizeof(Block), "Block(imports)");

                    Block *func = &m->functions[fidx];

                    func->import_module = import_module;
                    func->import_field = import_field;
                    func->type = &m->types[type_index];
                    trace("  import: %s.%s, fidx: 0x%x, type_index: 0x%x\n",
                          func->import_module, func->import_field, fidx,
                          type_index);

                    func->func_ptr = (void *(*)()) val;
                    break;
                case 0x01:             // Table
                    ASSERT(!m->table.entries, "More than 1 table not supported\n");
                    m->table.entries = val;
                    trace("  setting table.entries to: %p\n",
                          (void *) (*(uint32_t **) val));
                    m->table.entries = *(uint32_t **) val;
                    break;
                case 0x02:             // Memory
                    ASSERT(!m->memory.bytes, "More than 1 memory not supported\n");
                    trace(
                        "  setting memory.bytes to: %p\n", (void *) (*(uint8_t **) val));
                    m->memory.bytes = *(uint8_t **) val;
                    break;
                case 0x03:             // Global
                    m->global_count += 1;
                    m->globals = arecalloc(m->globals,
                                           m->global_count - 1, m->global_count,
                                           sizeof(StackValue), "globals");
                    StackValue *glob = &m->globals[m->global_count - 1];

                    glob->value_type = content_type;

                    switch (content_type) {
                    case I32:
                        memcpy(&glob->value.uint32, val, 4);
                        break;
                    case I64:
                        memcpy(&glob->value.uint64, val, 8);
                        break;
                    case F32:
                        memcpy(&glob->value.f32, val, 4);
                        break;
                    case F64:
                        memcpy(&glob->value.f64, val, 8);
                        break;
                    }
                    trace("    setting global %d (content_type %d) to %p: %s\n",
                          m->global_count - 1, content_type, val, value_repr(glob));
                    break;
                default:
                    FATAL("Import of kind %d not supported\n", external_kind);
                }

            }
            break;
        case 3:
            trace("Parsing Function(3) section (length: 0x%x)\n", slen);
            m->function_count += read_LEB(bytes, &pos, 32);
            trace("  import_count: %d, new count: %d\n",
                  m->import_count, m->function_count);

            Block *functions;

            functions = arecalloc(m->functions, m->import_count, m->function_count, sizeof(Block),
                                  "Block(function)");
            /*if (m->import_count != 0) {*/
            /*memcpy(functions, m->functions, sizeof(Block) * m->import_count);*/
            /*}*/
            /*if(m->functions)*/
            /*free(m->functions);*/
            m->functions = functions;

            for (uint32_t f = m->import_count; f < m->function_count; f++) {
                uint32_t tidx = read_LEB(bytes, &pos, 32);

                m->functions[f].fidx = f;
                m->functions[f].type = &m->types[tidx];
                trace("  function fidx: 0x%x, tidx: 0x%x\n", f, tidx);
            }
            break;
        case 4:
            trace("Parsing Table(4) section\n");
            uint32_t table_count = read_LEB(bytes, &pos, 32);

            trace("  table count: 0x%x\n", table_count);
            ASSERT(table_count == 1, "More than 1 table not supported");

            // Allocate the table
            // for (uint32_t c=0; c<table_count; c++) {
            parse_table_type(m, &pos);
            m->table.entries = acalloc(m->table.size,
                                       sizeof(uint32_t), "Module->table.entries");
            // }
            break;
        case 5:
            trace("Parsing Memory(5) section\n");
            uint32_t memory_count = read_LEB(bytes, &pos, 32);

            trace("  memory count: 0x%x\n", memory_count);
            ASSERT(memory_count == 1, "More than 1 memory not supported\n");

            // Allocate memory
            // for (uint32_t c=0; c<memory_count; c++) {
            parse_memory_type(m, &pos);
            m->memory.bytes = acalloc(m->memory.pages * pow(2, 16),
                                      sizeof(uint32_t), "Module->memory.bytes");
            // }
            break;
        case 6:
            trace("Parsing Global(6) section\n");
            uint32_t global_count = read_LEB(bytes, &pos, 32);

            for (uint32_t g = 0; g < global_count; g++) {
                // Same allocation Import of global above
                uint8_t type = read_LEB(bytes, &pos, 7);
                uint8_t mutability = read_LEB(bytes, &pos, 1);
                (void) mutability;
                uint32_t gidx = m->global_count;

                m->global_count += 1;
                m->globals = arecalloc(m->globals, gidx, m->global_count,
                                       sizeof(StackValue), "globals");
                m->globals[gidx].value_type = type;

                // Run the init_expr to get global value
                run_init_expr(m, type, &pos);

                m->globals[gidx] = m->stack[m->sp--];
            }
            pos = start_pos + slen;
            break;
        case 7:
            trace("Parsing Export(7) section (length: 0x%x)\n", slen);
            uint32_t export_count = read_LEB(bytes, &pos, 32);

            for (uint32_t e = 0; e < export_count; e++) {
                char *name = read_string(bytes, &pos, NULL);

                uint32_t kind = bytes[pos++];
                uint32_t index = read_LEB(bytes, &pos, 32);

                if (kind != 0x00) {
                    trace("  ignoring non-function export '%s'"
                          " kind 0x%x index 0x%x\n", name, kind, index);
                    free(name);
                    continue;
                }
                m->functions[index].export_name = name;
                trace("  export: %s (0x%x)\n", name, index);
            }
            break;
        case 8:
            trace("Parsing Start(8) section (length: 0x%x)\n", slen);
            m->start_function = read_LEB(bytes, &pos, 32);
            break;
        case 9:
            trace("Parsing Element(9) section (length: 0x%x)\n", slen);
            uint32_t element_count = read_LEB(bytes, &pos, 32);

            for (uint32_t c = 0; c < element_count; c++) {
                uint32_t index = read_LEB(bytes, &pos, 32);

                ASSERT(index == 0, "Only 1 default table in MVP");

                // Run the init_expr to get offset
                run_init_expr(m, I32, &pos);

                uint32_t offset = m->stack[m->sp--].value.uint32;

                if (m->options.mangle_table_index) {
                    // offset is the table address + the index (not sized for the
                    // pointer size) so get the actual (sized) index
                    trace("   origin offset offset: 0x%x, new offset: 0x%lx\n",
                          offset, offset - (uint64_t) m->table.entries);
                    offset = offset - (uint64_t) m->table.entries;
                }

                uint32_t num_elem = read_LEB(bytes, &pos, 32);

                trace("  table.entries: %p, offset: 0x%x\n", (void *) m->table.entries, offset);
                ASSERT(offset + num_elem <= m->table.size,
                       "table overflow %d+%d > %d\n", offset, num_elem, m->table.size);
                for (uint32_t n = 0; n < num_elem; n++) {
                    trace(
                        "  write table entries %p, offset: 0x%x, n: 0x%x, addr: %p\n",
                        (void *) m->table.entries, offset, n, (void *) &m->table.entries[offset + n]);
                    m->table.entries[offset + n] = read_LEB(bytes, &pos, 32);
                }
            }
            pos = start_pos + slen;
            break;
        // 9 and 11 are similar so keep them together, 10 is below 11
        case 11:
            trace("Parsing Data(11) section (length: 0x%x)\n", slen);
            uint32_t seg_count = read_LEB(bytes, &pos, 32);

            for (uint32_t s = 0; s < seg_count; s++) {
                uint32_t midx = read_LEB(bytes, &pos, 32);

                ASSERT(midx == 0, "Only 1 default memory in MVP");

                // Run the init_expr to get the offset
                run_init_expr(m, I32, &pos);

                uint32_t offset = m->stack[m->sp--].value.uint32;

                // Copy the data to the memory offset
                uint32_t size = read_LEB(bytes, &pos, 32);

                trace("  Get offset 0x%x, size %d\n", offset, size);
                ASSERT(offset + size <= m->memory.pages * pow(2, 16),
                       "memory overflow %d+%d > %d\n", offset, size,
                       (uint32_t) (m->memory.pages * pow(2, 16)));
                trace(
                    "  setting 0x%x bytes of memory at offset 0x%x\n", size, offset);
                memcpy(m->memory.bytes + offset, bytes + pos, size);
                pos += size;
            }

            break;
        case 10:
            trace("Parsing Code(10) section (length: 0x%x)\n", slen);
            uint32_t body_count = read_LEB(bytes, &pos, 32);

            for (uint32_t b = 0; b < body_count; b++) {
                Block *function = &m->functions[m->import_count + b];
                uint32_t body_size = read_LEB(bytes, &pos, 32);
                uint32_t payload_start = pos;
                uint32_t local_count = read_LEB(bytes, &pos, 32);
                uint32_t save_pos, tidx, lidx, lecount, code_start, code_end;
                // TODO: make use of this
                (void) code_start;
                (void) code_end;
                (void) tidx;

                // Local variable handling

                // Get number of locals for alloc
                save_pos = pos;
                function->local_count = 0;
                for (uint32_t l = 0; l < local_count; l++) {
                    lecount = read_LEB(bytes, &pos, 32);
                    function->local_count += lecount;
                    tidx = read_LEB(bytes, &pos, 7);
                }
                function->locals = acalloc(function->local_count,
                                           sizeof(uint32_t), "function->locals");

                // Restore position and read the locals
                pos = save_pos;
                lidx = 0;
                for (uint32_t l = 0; l < local_count; l++) {
                    lecount = read_LEB(bytes, &pos, 32);
                    vt = read_LEB(bytes, &pos, 7);
                    for (uint32_t l = 0; l < lecount; l++) {
                        function->locals[lidx++] = vt;
                    }
                }

                function->start_addr = pos;
                function->end_addr = payload_start + body_size - 1;
                function->br_addr = function->end_addr;
                ASSERT(bytes[function->end_addr] == 0x0b, "Code section did not end with 0x0b\n");
                pos = function->end_addr + 1;
            }
            break;
        default:
            FATAL("Section %d unimplemented\n", id);
            pos += slen;
        }

    }

    find_blocks(m);

    if (m->start_function > m->function_count && m->start_function < 0xFFFFFFFF) {
        uint32_t fidx = m->start_function;
        bool result;

        trace("Running start function 0x%x ('%s')\n", fidx, m->functions[fidx].export_name);

        if (TRACE && DEBUG) {
            dump_stacks(m);
        }

        if (fidx < m->import_count) {
            import_call(m, fidx);     // import/thunk call
        } else {
            setup_call(m, fidx);      // regular function call
        }

        if (m->csp < 0) {
            // start function was a direct external call
            result = true;
        } else {
            // run the function setup by setup_call
            result = interpret(m);
        }
        if (!result) {
            FATAL("Exception: %s\n", m->exception);
        }
    }

    return m;
}

void module_free(Module *m) {
    for (uint32_t i = 0; i < m->type_count; ++i) {
        Type *t = &m->types[i];
        free(t->params);
        free(t->results);
    }
    free(m->types);

    for (uint32_t i = 0; i < m->function_count; ++i) {
        Block *f = &m->functions[i];
        free(f->locals);
        free(f->export_name);
        free(f->import_module);
        free(f->import_field);
    }
    free(m->functions);

    for (uint32_t i = 0; i < m->byte_count; ++i) {
        Block *f = m->block_lookup[i];
        free(f);
    }
    free(m->block_lookup);

    free(m->globals);
    free(m->table.entries);
    free(m->memory.bytes);
    free(m);
}

bool invoke(Module *m, char *entry, size_t argc, StackValue *argv) {
    int32_t fidx = -1;
    Type *type;
    bool result;

    debug("Running function '%s'\n", entry);

    // Find entry function index
    if (entry) {
        fidx = get_export_fidx(m, entry);
    }
    if (fidx == -1) {
        warn("Function not found\n");
        return false;
    }
    type = m->functions[fidx].type;

    if (argc > 0 && argv == NULL) {
        warn("Argument count mismatch\n");
        return false;
    }

    // Check and add arguments to the stack
    for (size_t i = 0; i < argc; i++) {
        if (argv[i].value_type != type->params[i]) {
            warn("Argument type mismatch\n");
            return false;
        }
    }
    for (size_t i = 0; i < argc; i++) {
        m->sp++;
        m->stack[m->sp] = argv[i];
    }

    debug("Running function 0x%x ('%s')\n", fidx, entry);

    if (TRACE && DEBUG) {
        dump_stacks(m);
    }

    setup_call(m, fidx);

    result = interpret(m);

    if (TRACE && DEBUG) {
        dump_stacks(m);
    }

    return result;
}
