#include "Sim.hpp"
#include <elfio.hpp>
#include <iostream>

#define ELF_FILE_INFO_DUMP

Sim::Sim(const std::string& elf_filename) :
    registers(std::vector<uint32_t>(REG_NUM)) 
{   
    // problem - no space on my pc
    memspace = std::vector<uint8_t>(static_cast<size_t>(std::numeric_limits<uint32_t>::max()) + 2);


    ELFIO::elfio reader;
    if (!reader.load(elf_filename)) {
        throw std::invalid_argument("Can't open " + elf_filename);
    }

    assert(reader.get_class() == ELFIO::ELFCLASS32);
    assert(reader.get_encoding() == ELFIO::ELFDATA2LSB);

#ifdef ELF_FILE_INFO_DUMP
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
#endif

    pc = static_cast<uint32_t>(reader.get_entry());

    auto segments_num = reader.segments.size();

    for (size_t i = 0; i < segments_num; ++i) {

        const auto& segment = reader.segments[i];
        if (segment->get_type() != ELFIO::PT_LOAD) {
            continue;
        }

        const uint8_t* segment_data = reinterpret_cast<const uint8_t*>(segment->get_data());
        assert(segment_data);

        std::memcpy(memspace.data() + static_cast<uint32_t>(segment->get_virtual_address()), 
                    reinterpret_cast<const char *>(segment_data),
                    static_cast<size_t>(segment->get_file_size()) * sizeof(uint8_t));
    }



}