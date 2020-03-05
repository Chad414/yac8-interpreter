//
// Created by chad on 3/1/20.
//

#include "../include/Disassembler.h"
#include <sstream>
using namespace std;

/**
 * Hex Dump given ROM from filepath to
 *  output stream
 * 
 * @param filePath - The Path to the ROM
 * @param out - Output Stream of Hex Dump Output
 */
void Disassembler::hexDump(char *filePath, std::ostream &out) {
    u_char buffer[1000];
    int addr = 0;
    int n;
    std::ifstream infile;
    infile.open(filePath);

    // Check if file exists
    if (!infile) {
        cout << "File not found" << endl;
        return;
    }

    while (true) {
        infile.read((char *)buffer, 16);
        // Return buffer size up to 16
        n = infile.gcount();
        if (n <= 0) {
            break;
        }
        // Offset 16 bytes per line
        addr += 16;
        // Print line of n bytes
        for (int i = 0; i < 16; i++) {
            if (i + 1 <= n) {
                out << hex << setw(2) << setfill('0') << (int)buffer[i];
            }
            // Space each byte
            out << " ";
        }
        // New line after n bytes
        out << "\n";
        // Break if end of file
        if (infile.eof()) {
            break;
        }
    }
}

void Disassembler::disassemble(char *filePath, std::ostream &out) {
    stringstream ss;
    this->hexDump(filePath, ss);

    // Format Cout Flags
    ios_base::fmtflags prevFlags(out.flags()); // To restore Cout Output Style
    out << hex << uppercase << setw(2) << setfill('0');

    unsigned short PC = 0x200; // ROM Begins at 0x200
    unsigned short opcode;     // Instruction Index
    unsigned short param;      // Opcode's Paramater
    unsigned short addr;       // Temp Hold Addr/Const
    while (ss >> opcode) {
        ss >> param;
        out << setw(4) << setfill('0') << PC << '\t';
        out << setw(2) << setfill('0') << opcode << ' ' << setw(2) << setfill('0') << param << '\t';

        switch (opcode & 0xF0) { // Based on the First Nibble
        case 0x00:               // System Call (SYS addr)
            addr = ((opcode & 0x0F) << 8) | param;

            if (addr == 0x0E0) { // Clear Screen (CLS)
                out << "CLS";
            } else if (addr == 0x0EE) { // Return from Subroutine (RET)
                out << "RET";
            } else { // Output Data as in on Line
                out << setw(4) << ((opcode << 8) | param);
                break;
            }
            break;

        case 0x10: // Jump to Address (JP addr)
            // NNN = Address
            addr = ((opcode & 0x0F) << 8) | param;
            out << "JP " << addr;
            break;

        case 0x20: // Calls Subroutine (CALL addr)
            // NNN = Address
            addr = ((opcode & 0x0F) << 8) | param;
            out << "CALL " << addr;
            break;

        case 0x30: // Skip next Instruction if(reg[x] == NN) (SE Vx, byte)
            out << "SE V" << short(opcode & 0x0F);

            // Obtain Constant Byte
            out << ", " << param;
            break;

        case 0x40: // Skip next if (reg[x] != NN) (SNE Vx, byte)
            out << "SNE V" << short(opcode & 0x0F);

            // Obtain Constant Byte
            out << ", " << param;
            break;

        case 0x50: // Skip next if (reg[x] == reg[y]) (SE Vx, Vy)
            out << "SE V" << short(opcode & 0x0F);

            // Obtain next Register Byte
            out << ", V" << short((0xF0 & param) >> 4); // Reg[Y] -> 0x5XY0
            break;

        case 0x60: // Set reg[x] = NN (LD Vx, byte)
            out << "LD V" << short(opcode & 0x0F);

            // Obtain Constant
            out << ", " << param;
            break;

        case 0x70: // Adds reg[x] += NN (ADD Vx, byte)
            out << "ADD V" << short(opcode & 0x0F);

            // Obtain Constant
            out << ", " << param;
            break;

        case 0x80: // Register on Register Operations
            // Get Operation Type | 8x[y0]
            switch (param & 0x0F) { // Operation Type
            case 0x0:               // Set reg[x] = reg[y]
                out << "LD V" << short(opcode & 0x0F);
                out << ", V" << short(param & 0xF0);
                break;
            case 0x1: // Set reg[x] |= reg[y]
                out << "OR V" << short(opcode & 0x0F);
                out << ", V" << short(param & 0xF0);
                break;
            case 0x2: // Set reg[x] &= reg[y]
                out << "AND V" << short(opcode & 0x0F);
                out << ", V" << short(param & 0xF0);
                break;
            case 0x3: // Set reg[x] ^= reg[y]
                out << "XOR V" << short(opcode & 0x0F);
                out << ", V" << short(param & 0xF0);
                break;
            case 0x4: // Set reg[x] += reg[y]
                out << "ADD V" << short(opcode & 0x0F);
                out << ", V" << short(param & 0xF0);
                break;
            case 0x5: // Set reg[x] -= reg[y]
                out << "SUB V" << short(opcode & 0x0F);
                out << ", V" << short(param & 0xF0);
                break;
            case 0x6: // Shift reg[x] >>= 1
                out << "SHR V" << short(opcode & 0x0F);
                break;

            case 0x7: // Set reg[x] = reg[y] - reg[x]
                out << "SUBN V" << short(opcode & 0x0F);
                out << ", V" << short(param & 0xF0);
                break;

            case 0xE: // Shift regx[] <<= 1
                out << "SHL V" << short(opcode & 0x0F);
                break;

            default:
                // Output Data as in on Line
                // opcode = (opcode << 8) | addr;
                out << setw(4) << ((opcode << 8) | param);
                break;
            }
            break;

        case 0x90: // Skip next if (reg[x] != reg[y])
            out << "SNE V" << short(opcode & 0x0F);

            // Obtain next Register Byte
            out << ", V" << short(param & 0xF0);
            break;

        case 0xA0: // Set Register I = addr
            out << "LD I, ";

            // Get Address Annn
            addr = ((opcode & 0x0F) << 8) | param;
            out << addr;
            break;

        case 0xB0: // Jumps to location in addr + Reg[0]
            out << "JP V0, ";

            // Get Address Bnnn
            addr = ((opcode & 0x0F) << 8) | param;
            out << addr;
            break;

        case 0xC0: // Sets reg[x] = byte
            out << "RND V" << short(opcode & 0x0F);

            // Get Const Byte
            out << ", " << param;
            break;

        case 0xD0: // Draw n-byte sprite at mem (reg[x], reg[y])
            out << "DRW V" << short(opcode & 0x0F);

            // Get next Byte
            out << ", V" << short((param & 0xF0) >> 4) // Vy
                << ", " << short(param & 0x0F);        // n
            break;

        case 0xE0: // Skip/No-Skip next Instruction if Key in reg[x] is pressed
            // Get Next Byte (Skip/No-Skip)
            if (param == 0x9E) { // Skip
                out << "SKP V" << short(opcode & 0x0F);
            } else { // addr == 0xA1 (No Skip)
                out << "SKNP V" << short(opcode & 0x0F);
            }
            break;

        case 0xF0: // Timer | Key Press | Index Register | Sprite
            // Action Type based on Parameter
            switch (param) {
            case 0x07: // Delay Timer Value by DT
                out << "LD V" << short(opcode & 0x0F) << ", DT";
                break;

            case 0x0A: // Wait for Key Press and store Key in reg[x]
                out << "LD V" << short(opcode & 0x0F) << ", K";
                break;

            case 0x15: // Set Delay Timer to reg[x]
                out << "LD DT, V" << short(opcode & 0x0F);
                break;

            case 0x18: // Set Sound Timer to reg[x]
                out << "LD ST, V" << short(opcode & 0x0F);
                break;

            case 0x1E: // Set values of I to reg[x] I += reg[x]
                out << "ADD I, V" << short(opcode & 0x0F);
                break;

            case 0x29: // Set I to the location of Sprite Digit reg[x]
                out << "LD F, V" << short(opcode & 0x0F);
                break;

            case 0x33: // Store BCD rep of reg[x] in mem locaion I, I+1, and I+2
                out << "LD B, V" << short(opcode & 0x0F);
                break;

            case 0x55: // Store reg[0] to reg[x] in mem starting at location I
                out << "LD [I], V" << short(opcode & 0x0F);
                break;

            case 0x65: // Read reg[0] to reg[x] in mem starting at location I
                out << "LD V" << short(opcode & 0x0F)
                    << ", [I]";
                break;

            default:
                // Output Data as in on Line
                out << setw(4) << ((opcode << 8) | param);
                break;
            }


            break;



        default:
            // Output Data as in on Line
            out << setw(4) << ((opcode << 8) | param);
            break;
        }

        // Keep track of PC Line
        PC += 0x2;
        if (!(PC % 0x10)) out << '\n';

        out << '\n';
    }

    // Restore out Flags
    out.flags(prevFlags);
}