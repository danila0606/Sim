#include "Sim.hpp"
#include "Gfuncs.hpp"

#include <elfio.hpp>
#include <set>

//#define ELF_FILE_INFO_DUMP
//#define TRACE
//#define USE_CACHE

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

    for (int i = 0; i < segments_num; ++i) {

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

static Instruction decode(uint32_t word) {
    Instruction instr{};
    switch ((word >> 0) & 0b111000001111111) {
        case 0b11: {
            //! LB
            //! xxxxxxxxxxxxxxxxx000xxxxx0000011
            instr.id = Opcode::LB;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b1000000000011: {
            //! LH
            //! xxxxxxxxxxxxxxxxx001xxxxx0000011
            instr.id = Opcode::LH;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b10000000000011: {
            //! LW
            //! xxxxxxxxxxxxxxxxx010xxxxx0000011
            instr.id = Opcode::LW;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b100000000000011: {
            //! LBU
            //! xxxxxxxxxxxxxxxxx100xxxxx0000011
            instr.id = Opcode::LBU;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b101000000000011: {
            //! LHU
            //! xxxxxxxxxxxxxxxxx101xxxxx0000011
            instr.id = Opcode::LHU;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b1111: {
            //! FENCE
            //! xxxxxxxxxxxxxxxxx000xxxxx0001111
            instr.id = Opcode::FENCE;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 28>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<27, 24>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<23, 20>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            return instr;
        }
        case 0b10011: {
            //! ADDI
            //! xxxxxxxxxxxxxxxxx000xxxxx0010011
            instr.id = Opcode::ADDI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b10000000010011: {
            //! SLTI
            //! xxxxxxxxxxxxxxxxx010xxxxx0010011
            instr.id = Opcode::SLTI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b11000000010011: {
            //! SLTIU
            //! xxxxxxxxxxxxxxxxx011xxxxx0010011
            instr.id = Opcode::SLTIU;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b100000000010011: {
            //! XORI
            //! xxxxxxxxxxxxxxxxx100xxxxx0010011
            instr.id = Opcode::XORI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b110000000010011: {
            //! ORI
            //! xxxxxxxxxxxxxxxxx110xxxxx0010011
            instr.id = Opcode::ORI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b111000000010011: {
            //! ANDI
            //! xxxxxxxxxxxxxxxxx111xxxxx0010011
            instr.id = Opcode::ANDI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b100011: {
            //! SB
            //! xxxxxxxxxxxxxxxxx000xxxxx0100011
            instr.id = Opcode::SB;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            return instr;
        }
        case 0b1000000100011: {
            //! SH
            //! xxxxxxxxxxxxxxxxx001xxxxx0100011
            instr.id = Opcode::SH;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            return instr;
        }
        case 0b10000000100011: {
            //! SW
            //! xxxxxxxxxxxxxxxxx010xxxxx0100011
            instr.id = Opcode::SW;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            return instr;
        }
        case 0b1100011: {
            //! BEQ
            //! xxxxxxxxxxxxxxxxx000xxxxx1100011
            instr.id = Opcode::BEQ;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
            return instr;
        }
        case 0b1000001100011: {
            //! BNE
            //! xxxxxxxxxxxxxxxxx001xxxxx1100011
            instr.id = Opcode::BNE;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
            return instr;
        }
        case 0b100000001100011: {
            //! BLT
            //! xxxxxxxxxxxxxxxxx100xxxxx1100011
            instr.id = Opcode::BLT;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
            return instr;
        }
        case 0b101000001100011: {
            //! BGE
            //! xxxxxxxxxxxxxxxxx101xxxxx1100011
            instr.id = Opcode::BGE;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
            return instr;
        }
        case 0b110000001100011: {
            //! BLTU
            //! xxxxxxxxxxxxxxxxx110xxxxx1100011
            instr.id = Opcode::BLTU;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
            return instr;
        }
        case 0b111000001100011: {
            //! BGEU
            //! xxxxxxxxxxxxxxxxx111xxxxx1100011
            instr.id = Opcode::BGEU;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
            return instr;
        }
    }
    switch ((word >> 0) & 0b11111110000000000111000001111111) {
        case 0b1000000010011: {
            //! SLLI
            //! 0000000xxxxxxxxxx001xxxxx0010011
            instr.id = Opcode::SLLI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            return instr;
        }
        case 0b101000000010011: {
            //! SRLI
            //! 0000000xxxxxxxxxx101xxxxx0010011
            instr.id = Opcode::SRLI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            return instr;
        }
        case 0b1000000000000000101000000010011: {
            //! SRAI
            //! 0100000xxxxxxxxxx101xxxxx0010011
            instr.id = Opcode::SRAI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            return instr;
        }
        case 0b110011: {
            //! ADD
            //! 0000000xxxxxxxxxx000xxxxx0110011
            instr.id = Opcode::ADD;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b1000000110011: {
            //! SLL
            //! 0000000xxxxxxxxxx001xxxxx0110011
            instr.id = Opcode::SLL;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b10000000110011: {
            //! SLT
            //! 0000000xxxxxxxxxx010xxxxx0110011
            instr.id = Opcode::SLT;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b11000000110011: {
            //! SLTU
            //! 0000000xxxxxxxxxx011xxxxx0110011
            instr.id = Opcode::SLTU;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b100000000110011: {
            //! XOR
            //! 0000000xxxxxxxxxx100xxxxx0110011
            instr.id = Opcode::XOR;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b101000000110011: {
            //! SRL
            //! 0000000xxxxxxxxxx101xxxxx0110011
            instr.id = Opcode::SRL;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b110000000110011: {
            //! OR
            //! 0000000xxxxxxxxxx110xxxxx0110011
            instr.id = Opcode::OR;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b111000000110011: {
            //! AND
            //! 0000000xxxxxxxxxx111xxxxx0110011
            instr.id = Opcode::AND;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b10000000000110000000110011: {
            //! REM
            //! 0000001xxxxxxxxxx110xxxxx0110011
            instr.id = Opcode::REM;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b10000000000111000000110011: {
            //! REMU
            //! 0000001xxxxxxxxxx111xxxxx0110011
            instr.id = Opcode::REMU;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b1000000000000000000000000110011: {
            //! SUB
            //! 0100000xxxxxxxxxx000xxxxx0110011
            instr.id = Opcode::SUB;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b1000000000000000101000000110011: {
            //! SRA
            //! 0100000xxxxxxxxxx101xxxxx0110011
            instr.id = Opcode::SRA;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.rs2 = std::bit_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
            return instr;
        }
    }
    switch ((word >> 0) & 0b1111111) {
        case 0b10111: {
            //! AUIPC
            //! xxxxxxxxxxxxxxxxxxxxxxxxx0010111
            instr.id = Opcode::AUIPC;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 12>(word) << 12) >> 0;
            return instr;
        }
        case 0b110111: {
            //! LUI
            //! xxxxxxxxxxxxxxxxxxxxxxxxx0110111
            instr.id = Opcode::LUI;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 12>(word) << 12) >> 0;
            return instr;
        }
        case 0b1100111: {
            //! JALR
            //! xxxxxxxxxxxxxxxxx000xxxxx1100111
            instr.id = Opcode::JALR;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = std::bit_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b1101111: {
            //! JAL
            //! xxxxxxxxxxxxxxxxxxxxxxxxx1101111
            instr.id = Opcode::JAL;
            instr.rd = std::bit_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<31, 31>(word) << 31) >> 11;
            instr.imm |= std::bit_cast<int32_t>(slice<30, 21>(word) << 1) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<20, 20>(word) << 11) >> 0;
            instr.imm |= std::bit_cast<int32_t>(slice<19, 12>(word) << 12) >> 0;
            return instr;
        }
    }
    switch ((word >> 0) & 0b11111111111111111111111111111111) {
        case 0b1110011: {
            //! ECALL
            //! 00000000000000000000000001110011
            instr.id = Opcode::ECALL;
            return instr;
        }
        case 0b100000000000001110011: {
            //! EBREAK
            //! 00000000000100000000000001110011
            instr.id = Opcode::EBREAK;
            return instr;
        }
    }
    switch ((word >> 0) & 0b11111111111111111111000001111111) {
        case 0b1000000000000000001110011: {
            //! GRAPH_INIT
            //! 00000001000000000000000001110011
            instr.id = Opcode::GRAPH_INIT;
            return instr;
        }
        case 0b10000000000000000001110011: {
            //! GRAPH_PUT_PIXEL
            //! 00000010000000000000000001110011
            instr.id = Opcode::GRAPH_PUT_PIXEL;
            return instr;
        }
        case 0b11000000000000000001110011: {
            //! GRAPH_WINDOW_IS_OPEN
            //! 00000011000000000000000001110011
            instr.id = Opcode::GRAPH_WINDOW_IS_OPEN;
            return instr;
        }
        case 0b100000000000000000001110011: {
            //! GRAPH_TIME
            //! 00000100000000000000000001110011
            instr.id = Opcode::GRAPH_TIME;
            return instr;
        }
        case 0b101000000000000000001110011: {
            //! GRAPH_FLUSH
            //! 00000101000000000000000001110011
            instr.id = Opcode::GRAPH_FLUSH;
            return instr;
        }
        case 0b110000000000000000001110011: {
            //! GRAPH_RAND
            //! 00000110000000000000000001110011
            instr.id = Opcode::GRAPH_MUL;
            return instr;
        }
        case 0b111000000000000000001110011: {
            //! GRAPH_CLEAR
            //! 00000111000000000000000001110011
            instr.id = Opcode::GRAPH_DIV;
            return instr;
        }
    }

}   


void Sim::execute(Instruction instr) {

    uint8_t r1 = instr.rs1;
    uint8_t r2 = instr.rs2;
    uint8_t r3 = instr.rs3;

    uint8_t rd = instr.rd;

    int32_t imm = instr.imm;

    uint32_t tmp_32;
    uint16_t tmp_16;
    uint8_t  tmp_8;

    switch (instr.id)
    {
    case Opcode::ADD :
        registers[rd] = registers[r1] + registers[r2];
        pc += 4;
        break;
    case Opcode::ADDI :
        registers[rd] = registers[r1] + imm;
        pc += 4;
        break;
    case Opcode::SUB :
        registers[rd] = registers[r1] - registers[r2];
        pc += 4;
        break;
    case Opcode::XOR :
        registers[rd] = registers[r1] ^ registers[r2];
        pc += 4;
        break;
    case Opcode::XORI :
        registers[rd] = registers[r1] ^ static_cast<uint32_t>(imm);
        pc += 4;
        break;
    case Opcode::OR :
        registers[rd] = registers[r1] | registers[r2];
        pc += 4;
        break;
    case Opcode::ORI :
        registers[rd] = registers[r1] | static_cast<uint32_t>(imm);
        pc += 4;
        break;
    case Opcode::AND :
        registers[rd] = registers[r1] & registers[r2];
        pc += 4;
        break;
    case Opcode::ANDI :
        registers[rd] = registers[r1] & static_cast<uint32_t>(imm);
        pc += 4;
        break;
    case Opcode::AUIPC :
        registers[rd] = pc + imm;
        pc += 4;
        break;
    case Opcode::BEQ :
        pc += (r1 == r2) ? imm : 4;
        break;
    case Opcode::BGE :
        pc += (static_cast<int32_t>(registers[r1]) >= static_cast<int32_t>(registers[r2])) ? imm : 4;
        break;
    case Opcode::BGEU :
        pc += (registers[r1] >= registers[r2]) ? imm : 4;
        break;
    case Opcode::BLT :
        pc += (static_cast<int32_t>(registers[r1]) < static_cast<int32_t>(registers[r2])) ? imm : 4;
        break;
    case Opcode::BLTU :
        pc += (registers[r1] < registers[r2]) ? imm : 4;
        break;
    case Opcode::BNE :
        pc += (r1 != r2) ? imm : 4;
        break;
    case Opcode::EBREAK :
        program_halted = true;
        break;
    case Opcode::ECALL :
        program_halted = true;
        break;
    case Opcode::FENCE :
        pc =+ 4;
        break;
    case Opcode::FENCE_TSO :
        pc =+ 4;
        break;
    case Opcode::JAL :
        registers[rd] = pc + 4;
        pc += imm;
        break;
    case Opcode::JALR :
        {
            auto pc_next = pc + 4;
            pc = (static_cast<int32_t>(registers[r1]) + imm) & (~0b01);
            registers[rd] = pc_next;
            break;
        }
    case Opcode::LB :
        tmp_32 = *reinterpret_cast<const uint8_t*>(memspace.data() + (registers[r1] + imm));
        registers[rd] = static_cast<uint32_t>(tmp_32 << 24) >> 24;
        pc += 4;
        break;
    case Opcode::LBU :
        registers[rd] = 0xFF & (*reinterpret_cast<const uint8_t*>(memspace.data() + (registers[r1] + imm)));
        pc += 4;
        break;
    case Opcode::LH :
        tmp_32 = *reinterpret_cast<const uint16_t*>(memspace.data() + (registers[r1] + imm));
        registers[rd] = static_cast<uint32_t>(tmp_32 << 16) >> 16;
        pc += 4;
        break;
    case Opcode::LHU :
        registers[rd] = 0xFFFF & (*reinterpret_cast<const uint16_t*>(memspace.data() + (registers[r1] + imm)));
        pc += 4;
        break;
    case Opcode::LUI :
        registers[rd] = static_cast<uint32_t>(imm);
        pc += 4;
        break;
    case Opcode::LW :
        tmp_32 = *reinterpret_cast<const uint32_t*>(memspace.data() + (registers[r1] + imm));
        registers[rd] = tmp_32;
        pc += 4;
        break;
    case Opcode::PAUSE :
        program_halted = true;
        break;
    case Opcode::SB :
        tmp_8 = static_cast<uint8_t>(registers[r2] & 0xFF);
        *reinterpret_cast<uint8_t*>(memspace.data() + (registers[r1] + imm)) = tmp_8;
        pc += 4;
        break;
    case Opcode::SBREAK :
        program_halted = true;
        break;
    case Opcode::SCALL :
        program_halted = true;
        break;
    case Opcode::SH :
        tmp_16 = static_cast<uint16_t>(registers[r2] & 0xFFFF);
        *reinterpret_cast<uint16_t*>(memspace.data() + (registers[r1] + imm)) = tmp_16;
        pc += 4;
        break;
    case Opcode::SLL :
        registers[rd] = registers[r1] << (registers[r2] & 0b011111);
        pc += 4;
        break;
    case Opcode::SLT :
        registers[rd] = static_cast<int32_t>(registers[r1]) < static_cast<int32_t>(registers[r2]);
        pc += 4;
        break;
    case Opcode::SLTI :
        registers[rd] = static_cast<int32_t>(registers[r1]) < static_cast<int32_t>(imm);
        pc += 4;
        break;
    case Opcode::SLTIU :
        registers[rd] = static_cast<uint32_t>(registers[r1]) < static_cast<uint32_t>(imm);
        pc += 4;
        break;
    case Opcode::SLTU :
        registers[rd] = registers[r1] < registers[r2];
        pc += 4;
        break;
    case Opcode::SRL :
        registers[rd] = registers[r1] >> (registers[r2] & 0b011111);
        pc += 4;
        break;
    case Opcode::SRA :
        registers[rd] = (int)registers[r1] >> (registers[r2] & 0b011111);
        pc += 4;
        break;
    case Opcode::SW :
        tmp_32 = registers[r2];
        *reinterpret_cast<uint32_t*>(memspace.data() + (registers[r1] + imm)) = tmp_32;
        pc += 4;
        break;
    case Opcode::GRAPH_MUL :
        registers[15] = gmul((int)registers[10], (int)registers[11]);
        pc += 4;
        break;
    case Opcode::GRAPH_DIV :
        registers[15] = gdiv((int)registers[10], (int)registers[11]);
        pc += 4;
        break;
    case Opcode::GRAPH_INIT :
        init();
        pc += 4;
        break;
    case Opcode::GRAPH_FLUSH :
        flush();
        pc += 4;
        break;
    case Opcode::GRAPH_PUT_PIXEL :
        put_pixel(registers[10],registers[11],registers[12],registers[13],registers[14],registers[15]);
        pc += 4;
        break;
    case Opcode::GRAPH_WINDOW_IS_OPEN :
        registers[15] = is_window_opened();
        pc += 4;
        break;
    case Opcode::GRAPH_TIME :
        registers[15] = get_time_milliseconds();
        pc += 4;
        break;
    
    default:
        throw std::invalid_argument("Invalid Opcode: " + static_cast<int>(instr.id));
        break;
    }

    registers[0] = 0;
}

static bool is_end_of_block(Opcode opcode) {

    static std::set<Opcode> end_of_block_opcodes = {
        Opcode::BEQ, 
        Opcode::BGE,
        Opcode::BGEU,
        Opcode::BLT,
        Opcode::BLTU,
        Opcode::BNE,
        Opcode::EBREAK,
        Opcode::ECALL,
        Opcode::JAL,
        Opcode::JALR,
        Opcode::PAUSE,
        Opcode::SBREAK,
        Opcode::SCALL
    };

    return end_of_block_opcodes.count(opcode);
}

size_t Sim::run(std::ostream& trace_out) {

    size_t instr_count = 0;
    Instruction instr = {};

    while (!program_halted) {

#ifdef USE_CACHE
        uint32_t cashed_pc = pc; // start of block
        if (!simple_cache.count(pc)) {

            do {
                uint32_t word = *reinterpret_cast<const uint32_t*>(memspace.data() + pc);
                instr = decode(word);
                pc += 4;
                simple_cache[cashed_pc].push_back(instr);

            } while(!is_end_of_block(instr.id));

            pc = cashed_pc;     
        }

        std::vector<Instruction>& cached_instrs = simple_cache[pc];
        
        
        for (auto&& instr : cached_instrs)
        {   
            execute(instr);
        }

        instr_count += cached_instrs.size();
#else
        uint32_t word = *reinterpret_cast<const uint32_t*>(memspace.data() + pc);
        Instruction instr = decode(word);

        execute(instr);

        instr_count++;
#endif

#ifdef TRACE
    trace_out << "---------------------------------------------------------------" << std::endl;
    trace_out << std::dec
                  << int(instr.id) << " rd = " << (int)instr.rd
                  << ", rs1 = " << (int)instr.rs1
                  << ", rs2 = " << (int)instr.rs2
                  << ", rs3 = " << (int)instr.rs3 << std::hex << ", imm = 0x"
                  << instr.imm << std::dec << std::endl;

        trace_out << "PC = 0x" << std::hex << pc << std::endl;

        trace_out << "rd val" << std::hex << registers[instr.rd] << std::endl;
        trace_out << "Zero reg: " << registers[0] << std::endl;
        
#endif
    }

    return instr_count;
}

void Sim::dump_registers(std::ostream& out) {
    for (int i = 0; i < registers.size(); ++i) 
        out << std::dec << "r" << i << " : " << registers[i] << std::endl;
}