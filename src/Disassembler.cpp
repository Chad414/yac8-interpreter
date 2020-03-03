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
    unsigned char buffer[1000];
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
    ios_base::fmtflags prevFlags(cout.flags());  // To restore Cout Output Style
    out << hex << uppercase << setw(2) << setfill('0');

    int PC_line = 0x200;    // ROM Begins at 0x200
    int opcode;             // Instruction Index
    int addr;               // Temp Hold Addr/Const
    while (ss >> opcode) {
       out << setw(4) << setfill('0') << PC_line << '\t';

        switch (opcode & 0xF0) {  // Based on the First Nibble
        case 0x00:                // System Call (SYS addr)
            // NNN = Address
            addr = opcode & 0x0F;
            ss >> opcode;
            addr = opcode | (addr << 8);

            if (addr == 0x0E0) {  // Clear Screen (CLS)
                out << "CLS";
            } else if (addr == 0x0EE) {  // Return from Subroutine (RET)
                out << "RET";
            } else { // Output Data as in on Line
                opcode = (opcode << 8) | addr;
                out << setw(4) << opcode;
                break;
            }
            break;

        case 0x10:  // Jump to Address (JP addr)
            // NNN = Address
            addr = opcode & 0x0F;
            ss >> opcode;
            addr = opcode | (addr << 8);
            out << "JP " << addr;
            break;

        case 0x20:  // Calls Subroutine (CALL addr)
            // NNN = Address
            addr = opcode & 0x0F;
            ss >> opcode;
            addr = opcode | (addr << 8);
            out << "CALL " << addr;
            break;

        case 0x30:  // Skip next Instruction if(reg[x] == NN) (SE Vx, byte)
            out << "SE V" << int(opcode & 0x0F);

            // Obtain Constant Byte
            ss >> addr;
            out << ", " << addr;
            break;

        case 0x40:  // Skip next if (reg[x] != NN) (SNE Vx, byte)
            out << "SNE V" << int(opcode & 0x0F);

            // Obtain Constant Byte
            ss >> addr;
            out << ", " << addr;
            break;

        case 0x50:  // Skip next if (reg[x] == reg[y]) (SE Vx, Vy)
            out << "SE V" << int(opcode & 0x0F);

            // Obtain next Register Byte
            ss >> addr;
            out << ", V" << int(0xF0 & addr);  // Reg[Y] -> 0x5XY0
            break;

        case 0x60:  // Set reg[x] = NN (LD Vx, byte)
            out << "LD V" << int(opcode & 0x0F);

            // Obtain Constant
            ss >> addr;
            out << ", " << addr;
            break;

        case 0x70:  // Adds reg[x] += NN (ADD Vx, byte)
            out << "ADD V" << int(opcode & 0x0F);

            // Obtain Constant
            ss >> addr;
            out << ", " << addr;
            break;

        case 0x80:  // Register on Register Operations
            // Get Operation Type
            ss >> addr;  // 8x[y0]
            switch (addr & 0x0F) {  // Operation Type
            case 0x0:               // Set reg[x] = reg[y]
                out << "LD V" << int(opcode & 0x0F);
                out << ", V" << int(addr & 0xF0);
                break;
            case 0x1:  // Set reg[x] |= reg[y]
                out << "OR V" << int(opcode & 0x0F);
                out << ", V" << int(addr & 0xF0);
                break;
            case 0x2:  // Set reg[x] &= reg[y]
                out << "AND V" << int(opcode & 0x0F);
                out << ", V" << int(addr & 0xF0);
                break;
            case 0x3:  // Set reg[x] ^= reg[y]
                out << "XOR V" << int(opcode & 0x0F);
                out << ", V" << int(addr & 0xF0);
                break;
            case 0x4:  // Set reg[x] += reg[y]
                out << "ADD V" << int(opcode & 0x0F);
                out << ", V" << int(addr & 0xF0);
                break;
            case 0x5:  // Set reg[x] -= reg[y]
                out << "SUB V" << int(opcode & 0x0F);
                out << ", V" << int(addr & 0xF0);
                break;
            case 0x6:  // Shift reg[x] >>= 1
                out << "SHR V" << int(opcode & 0x0F);
                break;

            case 0x7:  // Set reg[x] = reg[y] - reg[x]
                out << "SUBN V" << int(opcode & 0x0F);
                out << ", V" << int(addr & 0xF0);
                break;

            case 0xE:  // Shift regx[] <<= 1
                out << "SHL V" << int(opcode & 0x0F);
                break;

            default:
                // Output Data as in on Line
                opcode = (opcode << 8) | addr;
                out << setw(4) << opcode;
                break;
            }


            out << "LD V" << int(opcode & 0x0F);

            // Obtain next Register Byte
            out << ", V" << int(0xF & addr);  // Reg[Y] -> 0x5XY0
            break;

        case 0x90:  // Skip next if (reg[x] != reg[y])
            out << "SNE V" << int(opcode & 0x0F);

            // Obtain next Register Byte
            ss >> addr;
            out << ", V" << int(addr & 0xF0);
            break;

        case 0xA0:  // Set Register I = addr
            out << "LD I, ";

            // Get Address
            addr = (opcode & 0x0F) << 8;

            // Get next Byte
            ss >> opcode;
            addr |= opcode;
            out << addr;

            break;

        case 0xB0:  // Jumps to location in addr + Reg[0]
            out << "JP V0, ";

            // Get Address
            addr = (opcode & 0x0F) << 8;

            // Get next Byte
            ss >> opcode;
            addr |= opcode;
            out << addr;

            break;

        case 0xC0:  // Sets reg[x] = byte
            out << "RND V" << int(opcode & 0x0F);

            // Get Const Byte
            ss >> addr;
            out << ", " << addr;

            break;

        case 0xD0:  // Draw n-byte sprite at mem (reg[x], reg[y])
            out << "DRW V" << int(opcode & 0x0F);

            // Get next Byte
            ss >> addr;
            out << ", V" << int((addr & 0xF0) >> 4)  // Vy
                 << ", " << int(addr & 0x0F);  // n

            break;

        case 0xE0:  // Skip/No-Skip next Instruction if Key in reg[x] is pressed
            // Get Next Byte (Skip/No-Skip)
            ss >> addr;

            if (addr == 0x9E) {  // Skip
                out << "SKP V" << int(opcode & 0x0F);
            } else {  // addr == 0xA1 (No Skip)
                out << "SKNP V" << int(opcode & 0x0F);
            }

            break;

        case 0xF0:  // Timer | Key Press | Index Register | Sprite
            // Get next Byte
            ss >> addr;

            // Action Type
            switch (addr) {
            case 0x07:  // Delay Timer Value by DT
                out << "LD V" << int(opcode & 0x0F) << ", DT";
                break;

            case 0x0A:  // Wait for Key Press and store Key in reg[x]
                out << "LD V" << int(opcode & 0x0F) << ", K";
                break;

            case 0x15:  // Set Delay Timer to reg[x]
                out << "LD DT, V" << int(opcode & 0x0F);
                break;

            case 0x18:  // Set Sound Timer to reg[x]
                out << "LD ST, V" << int(opcode & 0x0F);
                break;

            case 0x1E:  // Set values of I to reg[x] I += reg[x]
                out << "ADD I, V" << int(opcode & 0x0F);
                break;

            case 0x29:  // Set I to the location of Sprite Digit reg[x]
                out << "LD F, V" << int(opcode & 0x0F);
                break;

            case 0x33:  // Store BCD rep of reg[x] in mem locaion I, I+1, and I+2
                out << "LD B, V" << int(opcode & 0x0F);
                break;

            case 0x55:  // Store reg[0] to reg[x] in mem starting at location I
                out << "LD [I], V" << int(opcode & 0x0F);
                break;

            case 0x65:  // Read reg[0] to reg[x] in mem starting at location I
                out << "LD V" << int(opcode & 0x0F)
                     << ", [I]";
                break;

            default:
                // Output Data as in on Line
                opcode = (opcode << 8) | addr;
                out << setw(4) << opcode;
                break;
            }


            break;



        default:
            // Get next Byte
            ss >> addr;

            // Output Data as in on Line
            opcode = (opcode << 8) | addr;
            out << setw(4) << opcode;
            break;
        }

        // Keep track of PC Line
        PC_line += 0x2;
        if (!(PC_line % 0x10)) out << '\n';
        
        out << '\n';
    }

    // Restore out Flags
    out.flags(prevFlags);
}