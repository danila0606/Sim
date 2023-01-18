#include <iostream>

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

    return 0;
}