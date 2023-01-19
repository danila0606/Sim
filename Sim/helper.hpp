#pragma once

#include <cassert>

#define REG_NUM 32

#define OPCODE_MASK 0b1111111
#define OPCODE_SHIFT 0

#define FUNCT3_MASK 0b111
#define FUNCT3_SHIFT 12

#define LOAD_MASK 0b11

constexpr uint32_t ones(uint32_t num) { return (1 << num) - 1; }

template <std::size_t msb, std::size_t lsb> constexpr uint32_t get_mask() {
  static_assert(msb >= lsb, "Error : illegal bits range");
  return ones(msb - lsb + 1) << lsb;
}

template <std::size_t msb, std::size_t lsb>
constexpr uint32_t slice(uint32_t word) {
  static_assert(msb >= lsb, "Error : illegal bits range");
  return (word & get_mask<msb, lsb>()) >> lsb;
}
