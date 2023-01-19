#include <iostream>
#include <chrono>

#include "Sim/Sim.hpp"

#define MY_DEBUG

int main(int argc, char **argv) {

#ifdef MY_DEBUG
    std::string elf_filename = std::string("../elf_examples/a.out");
#else
    if (argc != 2) {
        std::cout << "Must be 1 arg - name of elf file" << std::endl;
        return -1;
    }

    std::string elf_filename = std::string(argv[1]);
#endif

    Sim sim(elf_filename);

    try {
        
        auto start = std::chrono::steady_clock::now();
        size_t instr_count = sim.run();
        auto finish = std::chrono::steady_clock::now();

        double seconds = (finish - start).count() / 1000000000;

        sim.dump_registers();

        std::cout << "Instruction num: "<< instr_count << std::endl;

        std::cout << "Time: " << seconds << std::endl;
        std::cout << "Mips: " << static_cast<double>(instr_count) / (seconds * 1000000) << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << e.what() <<std::endl;
        exit(-1);
    }

    return 0;
}