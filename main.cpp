#include <iostream>
#include <string>

#include <elfio.hpp>


int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Must be 1 arg - name of elf file" << std::endl;
        return -1;
    }

    // Create elfio reader
    ELFIO::elfio reader;
    // Load ELF data
    if ( !reader.load( argv[1] ) ) {
        std::cout << "Can't find or process ELF file " << argv[1] << std::endl;
        return 2;
    }

    // Print ELF file properties
    std::cout << "ELF file class : ";
    if ( reader.get_class() == ELFIO::ELFCLASS32 )
        std::cout << "ELF32" << std::endl;
    else
        std::cout << "ELF64" << std::endl;

    std::cout << "ELF file encoding : ";
    if ( reader.get_encoding() == ELFIO::ELFDATA2LSB )
        std::cout << "Little endian" << std::endl;
    else
        std::cout << "Big endian" << std::endl;

    ELFIO::Elf_Half sec_num = reader.sections.size();
    std::cout << "Number of sections: " << sec_num << std::endl;
    for ( int i = 0; i < sec_num; ++i ) {
        const ELFIO::section* psec = reader.sections[i];
        std::cout << " [" << i << "] "
            << psec->get_name()
            << "\t"
            << psec->get_size()
            << std::endl;
        // Access section's data
        
        const char* p = reader.sections[i]->get_data();
    }

    ELFIO::Elf_Half seg_num = reader.segments.size();
    std::cout << "Number of segments: " << seg_num << std::endl;
    for ( int i = 0; i < seg_num; ++i ) {
    const ELFIO::segment* pseg = reader.segments[i];
    std::cout << " [" << i << "] 0x" << std::hex
        << pseg->get_flags()
        << "\t0x"
        << pseg->get_virtual_address()
        << "\t0x"
        << pseg->get_file_size()
        << "\t0x"
        << pseg->get_memory_size()
        << std::endl;
    // Access segments's data
    const char* p = reader.segments[i]->get_data();
    }

    return 0;
}