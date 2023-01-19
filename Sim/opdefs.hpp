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
    SRA,
    SRL,
    SUB,
    SW,
    XOR,
    XORI,
};

struct Instruction {

    uint8_t rs1= {}, rs2 = {}, rs3 = {};
    uint8_t rd = {};

    uint32_t imm = {};
    Opcode id = Opcode::NONE;
};