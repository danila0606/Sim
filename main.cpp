#include <chrono>
#include <fstream>

#include "Sim/Sim.hpp"

//#define MY_DEBUG
//#define TRACE

// cmake -DCMAKE_BUILD_TYPE=Release ..
// ../riscv32-embecosm-ubuntu2204-gcc12.2.0/bin/riscv32-unknown-elf-gcc -march=rv32i br.c -O0 -e main

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

#ifdef TRACE
    std::string out_file_name("../dump.txt");
    std::ofstream trace_out_file(out_file_name);
    if (!trace_out_file.is_open()) {
        std::cerr << "Can't open file" << std::endl;
        exit(-1);
    }
#else
    std::ostream& trace_out_file = std::cout;
#endif

    Sim sim(elf_filename);

    try {
        
        auto start = std::chrono::steady_clock::now();
        size_t instr_count = sim.run(trace_out_file);
        auto finish = std::chrono::steady_clock::now();

        double seconds = static_cast<double>((finish - start).count()) / 1000000000;

        sim.dump_registers(trace_out_file);

#ifdef TRACE
        trace_out_file.close();
#endif

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