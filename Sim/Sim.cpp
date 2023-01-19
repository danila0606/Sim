#include "Sim.hpp"

#include <elfio.hpp>

//#define ELF_FILE_INFO_DUMP

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

// static Instruction decode(uint32_t word) {

//     Instruction instr{};

//     uint32_t opcode = (word >> OPCODE_SHIFT) & OPCODE_MASK;

//     switch (opcode) {
//         case LOAD_MASK : {
//             uint32_t width = (word >> FUNCT3_SHIFT) & FUNCT3_MASK;

//             switch (width) {
//                 case 0b0:
//                 {
                    
//                 }
//                 case 0b1:
//                 {
                    
//                 }
//                 case 0b10:
//                 {
                    
//                 }
//                 case 0b100:
//                 {
                    
//                 }
//                 case 0b101:
//                 {
                    
//                 }
//             } 
//         }
//     }


// }

static Instruction decode(uint32_t word) {
    Instruction instr{};
    switch ((word >> 0) & 0b1111111) {
        case 0b11: {
            switch ((word >> 12) & 0b111) {
                case 0b0: {
                    //! LB
                    //! xxxxxxxxxxxxxxxxx000xxxxx0000011
                    instr.id = Opcode::LB;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b1: {
                    //! LH
                    //! xxxxxxxxxxxxxxxxx001xxxxx0000011
                    instr.id = Opcode::LH;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b10: {
                    //! LW
                    //! xxxxxxxxxxxxxxxxx010xxxxx0000011
                    instr.id = Opcode::LW;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b100: {
                    //! LBU
                    //! xxxxxxxxxxxxxxxxx100xxxxx0000011
                    instr.id = Opcode::LBU;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b101: {
                    //! LHU
                    //! xxxxxxxxxxxxxxxxx101xxxxx0000011
                    instr.id = Opcode::LHU;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
            }
        }
        case 0b1111: {
            switch ((word >> 12) & 0b111) {
                case 0b0: {
                    //! FENCE
                    //! xxxxxxxxxxxxxxxxx000xxxxx0001111
                    instr.id = Opcode::FENCE;
                    instr.imm |= static_cast<int32_t>(slice<31, 28>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<27, 24>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<23, 20>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    return instr;
                }
            }
        }
        case 0b10011: {
            switch ((word >> 12) & 0b111) {
                case 0b0: {
                    //! ADDI
                    //! xxxxxxxxxxxxxxxxx000xxxxx0010011
                    instr.id = Opcode::ADDI;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b10: {
                    //! SLTI
                    //! xxxxxxxxxxxxxxxxx010xxxxx0010011
                    instr.id = Opcode::SLTI;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b11: {
                    //! SLTIU
                    //! xxxxxxxxxxxxxxxxx011xxxxx0010011
                    instr.id = Opcode::SLTIU;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b100: {
                    //! XORI
                    //! xxxxxxxxxxxxxxxxx100xxxxx0010011
                    instr.id = Opcode::XORI;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b110: {
                    //! ORI
                    //! xxxxxxxxxxxxxxxxx110xxxxx0010011
                    instr.id = Opcode::ORI;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
                case 0b111: {
                    //! ANDI
                    //! xxxxxxxxxxxxxxxxx111xxxxx0010011
                    instr.id = Opcode::ANDI;
                    instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
                    return instr;
                }
            }
        }
        case 0b10111: {
            //! AUIPC
            //! xxxxxxxxxxxxxxxxxxxxxxxxx0010111
            instr.id = Opcode::AUIPC;
            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.imm |= static_cast<int32_t>(slice<31, 12>(word) << 12) >> 0;
            return instr;
        }
        case 0b100011: {
            switch ((word >> 12) & 0b111) {
                case 0b0: {
                    //! SB
                    //! xxxxxxxxxxxxxxxxx000xxxxx0100011
                    instr.id = Opcode::SB;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    return instr;
                }
                case 0b1: {
                    //! SH
                    //! xxxxxxxxxxxxxxxxx001xxxxx0100011
                    instr.id = Opcode::SH;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    return instr;
                }
                case 0b10: {
                    //! SW
                    //! xxxxxxxxxxxxxxxxx010xxxxx0100011
                    instr.id = Opcode::SW;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                    return instr;
                }
            }
        }
        case 0b110011: {
            switch ((word >> 25) & 0b1111111) {
                case 0b0: {
                    switch ((word >> 12) & 0b111) {
                        case 0b0: {
                            //! ADD
                            //! 0000000xxxxxxxxxx000xxxxx0110011
                            instr.id = Opcode::ADD;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b1: {
                            //! SLL
                            //! 0000000xxxxxxxxxx001xxxxx0110011
                            instr.id = Opcode::SLL;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b10: {
                            //! SLT
                            //! 0000000xxxxxxxxxx010xxxxx0110011
                            instr.id = Opcode::SLT;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b11: {
                            //! SLTU
                            //! 0000000xxxxxxxxxx011xxxxx0110011
                            instr.id = Opcode::SLTU;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b100: {
                            //! XOR
                            //! 0000000xxxxxxxxxx100xxxxx0110011
                            instr.id = Opcode::XOR;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b101: {
                            //! SRL
                            //! 0000000xxxxxxxxxx101xxxxx0110011
                            instr.id = Opcode::SRL;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b110: {
                            //! OR
                            //! 0000000xxxxxxxxxx110xxxxx0110011
                            instr.id = Opcode::OR;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b111: {
                            //! AND
                            //! 0000000xxxxxxxxxx111xxxxx0110011
                            instr.id = Opcode::AND;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                    }
                }
                case 0b100000: {
                    switch ((word >> 12) & 0b111) {
                        case 0b0: {
                            //! SUB
                            //! 0100000xxxxxxxxxx000xxxxx0110011
                            instr.id = Opcode::SUB;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                        case 0b101: {
                            //! SRA
                            //! 0100000xxxxxxxxxx101xxxxx0110011
                            instr.id = Opcode::SRA;
                            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
                            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                            instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                            return instr;
                        }
                    }
                }
            }
        }
        case 0b110111: {
            //! LUI
            //! xxxxxxxxxxxxxxxxxxxxxxxxx0110111
            instr.id = Opcode::LUI;
            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.imm |= static_cast<int32_t>(slice<31, 12>(word) << 12) >> 0;
            return instr;
        }
        case 0b1100011: {
            switch ((word >> 12) & 0b111) {
                case 0b0: {
                    //! BEQ
                    //! xxxxxxxxxxxxxxxxx000xxxxx1100011
                    instr.id = Opcode::BEQ;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
                    return instr;
                }
                case 0b1: {
                    //! BNE
                    //! xxxxxxxxxxxxxxxxx001xxxxx1100011
                    instr.id = Opcode::BNE;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
                    return instr;
                }
                case 0b100: {
                    //! BLT
                    //! xxxxxxxxxxxxxxxxx100xxxxx1100011
                    instr.id = Opcode::BLT;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
                    return instr;
                }
                case 0b101: {
                    //! BGE
                    //! xxxxxxxxxxxxxxxxx101xxxxx1100011
                    instr.id = Opcode::BGE;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
                    return instr;
                }
                case 0b110: {
                    //! BLTU
                    //! xxxxxxxxxxxxxxxxx110xxxxx1100011
                    instr.id = Opcode::BLTU;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
                    return instr;
                }
                case 0b111: {
                    //! BGEU
                    //! xxxxxxxxxxxxxxxxx111xxxxx1100011
                    instr.id = Opcode::BGEU;
                    instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 19;
                    instr.imm |= static_cast<int32_t>(slice<30, 25>(word) << 5) >> 0;
                    instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
                    instr.rs2 = static_cast<int32_t>(slice<24, 20>(word) << 0) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<11, 8>(word) << 1) >> 0;
                    instr.imm |= static_cast<int32_t>(slice<7, 7>(word) << 11) >> 0;
                    return instr;
                }
            }
        }
        case 0b1100111: {
            //! JALR
            //! xxxxxxxxxxxxxxxxx000xxxxx1100111
            instr.id = Opcode::JALR;
            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.rs1 = static_cast<int32_t>(slice<19, 15>(word) << 0) >> 0;
            instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 20;
            instr.imm |= static_cast<int32_t>(slice<30, 20>(word) << 0) >> 0;
            return instr;
        }
        case 0b1101111: {
            //! JAL
            //! xxxxxxxxxxxxxxxxxxxxxxxxx1101111
            instr.id = Opcode::JAL;
            instr.rd = static_cast<int32_t>(slice<11, 7>(word) << 0) >> 0;
            instr.imm |= static_cast<int32_t>(slice<31, 31>(word) << 31) >> 11;
            instr.imm |= static_cast<int32_t>(slice<30, 21>(word) << 1) >> 0;
            instr.imm |= static_cast<int32_t>(slice<20, 20>(word) << 11) >> 0;
            instr.imm |= static_cast<int32_t>(slice<19, 12>(word) << 12) >> 0;
            return instr;
        }
        case 0b1110011: {
            switch ((word >> 7) & 0b1111111111111111111111111) {
                case 0b0: {
                    //! ECALL
                    //! 00000000000000000000000001110011
                    instr.id = Opcode::ECALL;
                    return instr;
                }
                case 0b10000000000000: {
                    //! EBREAK
                    //! 00000000000100000000000001110011
                    instr.id = Opcode::EBREAK;
                    return instr;
                }
            }
        }
    }

}  //!

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
        registers[rd] = (static_cast<int32_t>(registers[r1]) + imm) & (~0b01);
        pc = imm;
        break;
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
    default:
        throw std::invalid_argument("Invalid Opcode: " + static_cast<int>(instr.id));
        break;
    }

    registers[0] = 0;
}


size_t Sim::run(std::ostream& trace_out) {

    size_t instr_count = 0;
    Instruction instr = {};

    while (!program_halted) {

        if (!simple_cache.count(pc)) {
            uint32_t word = *reinterpret_cast<const uint32_t*>(memspace.data() + pc);

            instr = decode(word);

            simple_cache[pc] = instr;
        }
        else {
            instr = simple_cache[pc];
        }
        
        execute(instr);

#ifdef TRACE
    trace_out << "---------------------------------------------------------------" << std::endl;
    trace_out << int(instr.id)
                  << std::dec << " rd = " << (int)instr.rd
                  << ", rs1 = " << (int)instr.rs1
                  << ", rs2 = " << (int)instr.rs2
                  << ", rs3 = " << (int)instr.rs3 << std::hex << ", imm = 0x"
                  << instr.imm << std::dec << std::endl;

        trace_out << "PC = 0x" << std::hex << pc << std::endl;

        trace_out << "rd val" << std::hex << registers[instr.rd] << std::endl;
        trace_out << "Zero reg: " << registers[0] << std::endl;
        
#endif
        instr_count++;
    }

    return instr_count;
}

void Sim::dump_registers(std::ostream& out) {
    for (int i = 0; i < registers.size(); ++i) 
        out << std::dec << "r" << i << " : " << registers[i] << std::endl;
}