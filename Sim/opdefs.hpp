#pragma once

#include <ctype.h>

enum class Opcode {

    NONE = 0,
    ADD,
    ADDI,
    AND,
    ANDI,
    AUIPC,
    BEQ,
    BGE,
    BGEU,
    BLT,
    BLTU,
    BNE,
    EBREAK,
    ECALL,
    FENCE,
    FENCE_TSO,
    JAL,
    JALR,
    LB,
    LBU,
    LH,
    LHU,
    LUI,
    LW,
    OR,
    ORI,
    PAUSE,
    SB,
    SBREAK,
    SCALL,
    SH,
    SLL,
    SLT,
    SLTI,
    SLTIU,
    SLTU,
    SLLI,
    SRLI,
    SRAI,
    REM,
    REMU,
    SRA,
    SRL,
    SUB,
    SW,
    XOR,
    XORI,

    GRAPH_INIT,
    GRAPH_PUT_PIXEL,
    GRAPH_WINDOW_IS_OPEN,
    GRAPH_TIME,
    GRAPH_FLUSH,
    GRAPH_MUL,
    GRAPH_DIV
};

struct Instruction {

    uint8_t rs1= {}, rs2 = {}, rs3 = {};
    uint8_t rd = {};

    int32_t imm = {};
    Opcode id = Opcode::NONE;
};