#pragma once

#include <vector>
#include <string>

#include "helper.hpp" 

class Sim final {

public:

    Sim(const std::string& elf_filename);

public:

    void run();

private:
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memspace;    

private:

    uint32_t pc = 0;
    bool program_halted = false;
    
};