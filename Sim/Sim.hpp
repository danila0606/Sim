#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include "helper.hpp" 
#include "opdefs.hpp"

class Sim final {

public:

    Sim(const std::string& elf_filename);

public:

    size_t run();

public:

    void execute(Instruction instr);
    void dump_registers();

private:
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memspace;    

private:

    uint32_t pc = 0;
    bool program_halted = false;
    
};