#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

#include "helper.hpp" 
#include "opdefs.hpp"

//#define TRACE

class Sim final {

public:

    Sim(const std::string& elf_filename);

public:

    size_t run(std::ostream& out);

public:

    void execute(Instruction instr);
    void dump_registers(std::ostream& out);

private:
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memspace;    

private:

    uint32_t pc = 0;
    bool program_halted = false;

private:

    std::unordered_map<uint32_t, std::vector<Instruction>> simple_cache = {};
    
};