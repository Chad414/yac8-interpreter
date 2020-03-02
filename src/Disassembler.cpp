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

void Disassembler::disassemble(char *filePath) {
    stringstream ss;
    this->hexDump(filePath, ss);

    // Format Cout Flags
    ios_base::fmtflags prevFlags(cout.flags());  // To restore Cout Output Style
    cout << hex << uppercase << setw(2) << setfill('0');

    int opcode;       // Instruction Index
    int addr;         // Temp Hold Addr/Const
    int counter = 0;  // Counter to limit 16 Bytes per Line
    int line = 0;
    while (ss >> opcode) {
        switch (opcode & 0xF0) {  // Based on the First Nibble
        case 0x00:                // System Call (SYS addr)
            cout << "SYS ";

            // NNN = Address
            addr = opcode & 0x0F;
            ss >> opcode;
            addr = opcode | (addr << 8);

            if (addr == 0x0E0) {  // Clear Screen (CLS)
                cout << "CLS";
            } else if (addr == 0x0EE) {  // Return from Subroutine (RET)
                cout << "RET";
            } else {  // Some Address to Call
                cout << addr;
            }

            counter++;
            break;

        case 0x10:  // Jump to Address (JP addr)
            // NNN = Address
            addr = opcode & 0x0F;
            ss >> opcode;
            addr = opcode | (addr << 8);
            cout << "JP " << addr;
            counter++;
            break;

        case 0x20:  // Calls Subroutine (CALL addr)
            // NNN = Address
            addr = opcode & 0x0F;
            ss >> opcode;
            addr = opcode | (addr << 8);
            cout << "CALL " << addr;
            counter++;
            break;

        case 0x30:  // Skip next Instruction if(reg[x] == NN) (SE Vx, byte)
            cout << "SE V" << int(opcode & 0x0F);

            // Obtain Constant Byte
            ss >> addr;
            cout << ", " << addr;
            counter++;
            break;

        case 0x40:  // Skip next if (reg[x] != NN) (SNE Vx, byte)
            cout << "SNE V" << int(opcode & 0x0F);

            // Obtain Constant Byte
            ss >> addr;
            cout << ", " << addr;
            counter++;
            break;

        case 0x50:  // Skip next if (reg[x] == reg[y]) (SE Vx, Vy)
            cout << "SE V" << int(opcode & 0x0F);

            // Obtain next Register Byte
            ss >> addr;
            cout << ", V" << int(0xF0 & addr);  // Reg[Y] -> 0x5XY0
            counter++;
            break;

        case 0x60:  // Set reg[x] = NN (LD Vx, byte)
            cout << "LD V" << int(opcode & 0x0F);

            // Obtain Constant
            ss >> addr;
            cout << ", " << addr;
            counter++;
            break;

        case 0x70:  // Adds reg[x] += NN (ADD Vx, byte)
            cout << "ADD V" << int(opcode & 0x0F);

            // Obtain Constant
            ss >> addr;
            cout << ", " << addr;
            counter++;
            break;

        case 0x80:  // Register on Register Operations
            // Get Operation Type
            ss >> addr;  // 8x[y0]
            counter++;
            switch (addr & 0x0F) {  // Operation Type
            case 0x0:               // Set reg[x] = reg[y]
                cout << "LD V" << int(opcode & 0x0F);
                cout << ", V" << int(addr & 0xF0);
                break;
            case 0x1:  // Set reg[x] |= reg[y]
                cout << "OR V" << int(opcode & 0x0F);
                cout << ", V" << int(addr & 0xF0);
                break;
            case 0x2:  // Set reg[x] &= reg[y]
                cout << "AND V" << int(opcode & 0x0F);
                cout << ", V" << int(addr & 0xF0);
                break;
            case 0x3:  // Set reg[x] ^= reg[y]
                cout << "XOR V" << int(opcode & 0x0F);
                cout << ", V" << int(addr & 0xF0);
                break;
            case 0x4:  // Set reg[x] += reg[y]
                cout << "ADD V" << int(opcode & 0x0F);
                cout << ", V" << int(addr & 0xF0);
                break;
            case 0x5:  // Set reg[x] -= reg[y]
                cout << "SUB V" << int(opcode & 0x0F);
                cout << ", V" << int(addr & 0xF0);
                break;
            case 0x6:  // Shift reg[x] >>= 1
                cout << "SHR V" << int(opcode & 0x0F);
                break;

            case 0x7:  // Set reg[x] = reg[y] - reg[x]
                cout << "SUBN V" << int(opcode & 0x0F);
                cout << ", V" << int(addr & 0xF0);
                break;

            case 0xE:  // Shift regx[] <<= 1
                cout << "SHL V" << int(opcode & 0x0F);
                break;

            default:
                cerr << "\nERROR: 0xF0 Timer, Key Press, Index Reg, Sprite error!\n";
                cout << "Opcode: " << opcode;
                cout << "\nAddr: " << addr;

                // Restore Cout Flags
                cout.flags(prevFlags);
                return;
                break;
            }


            cout << "LD V" << int(opcode & 0x0F);

            // Obtain next Register Byte
            cout << ", V" << int(0xF & addr);  // Reg[Y] -> 0x5XY0
            break;

        case 0x90:  // Skip next if (reg[x] != reg[y])
            cout << "SNE V" << int(opcode & 0x0F);

            // Obtain next Register Byte
            ss >> addr;
            cout << ", V" << int(addr & 0xF0);
            counter++;
            break;

        case 0xA0:  // Set Register I = addr
            cout << "LD I, ";

            // Get Address
            addr = (opcode & 0x0F) << 8;

            // Get next Byte
            ss >> opcode;
            addr |= opcode;
            cout << addr;

            counter++;
            break;

        case 0xB0:  // Jumps to location in addr + Reg[0]
            cout << "JP V0, ";

            // Get Address
            addr = (opcode & 0x0F) << 8;

            // Get next Byte
            ss >> opcode;
            addr |= opcode;
            cout << addr;

            counter++;
            break;

        case 0xC0:  // Sets reg[x] = byte
            cout << "RND V" << int(opcode & 0x0F);

            // Get Const Byte
            ss >> addr;
            cout << ", " << addr;

            counter++;
            break;

        case 0xD0:  // Draw n-byte sprite at mem (reg[x], reg[y])
            cout << "DRW V" << int(opcode & 0x0F);

            // Get next Byte
            ss >> addr;
            cout << ", V" << int((addr & 0xF0) >> 4)  // Vy
                 << ", " << int(addr & 0x0F);  // n

            counter++;
            break;

        case 0xE0:  // Skip/No-Skip next Instruction if Key in reg[x] is pressed
            // Get Next Byte (Skip/No-Skip)
            ss >> addr;

            if (addr == 0x9E) {  // Skip
                cout << "SKP V" << int(opcode & 0x0F);
            } else {  // addr == 0xA1 (No Skip)
                cout << "SKNP V" << int(opcode & 0x0F);
            }

            counter++;
            break;

        case 0xF0:  // Timer | Key Press | Index Register | Sprite
            // Get next Byte
            ss >> addr;
            counter++;

            // Action Type
            switch (addr) {
            case 0x07:  // Delay Timer Value by DT
                cout << "LD V" << int(opcode & 0x0F) << ", DT";
                break;

            case 0x0A:  // Wait for Key Press and store Key in reg[x]
                cout << "LD V" << int(opcode & 0x0F) << ", K";
                break;

            case 0x15:  // Set Delay Timer to reg[x]
                cout << "LD DT, V" << int(opcode & 0x0F);
                break;

            case 0x18:  // Set Sound Timer to reg[x]
                cout << "LD ST, V" << int(opcode & 0x0F);
                break;

            case 0x1E:  // Set values of I to reg[x] I += reg[x]
                cout << "ADD I, V" << int(opcode & 0x0F);
                break;

            case 0x29:  // Set I to the location of Sprite Digit reg[x]
                cout << "LD F, V" << int(opcode & 0x0F);
                break;

            case 0x33:  // Store BCD rep of reg[x] in mem locaion I, I+1, and I+2
                cout << "LD B, V" << int(opcode & 0x0F);
                break;

            case 0x55:  // Store reg[0] to reg[x] in mem starting at location I
                cout << "LD [I], V" << int(opcode & 0x0F);
                break;

            case 0x65:  // Read reg[0] to reg[x] in mem starting at location I
                cout << "LD V" << int(opcode & 0x0F)
                     << ", [I]";
                break;

            default:
                cerr << "\nERROR: 0xF0 Timer, Key Press, Index Reg, Sprite error!\n";
                cout << "Opcode: " << opcode;
                cout << "\nAddr: " << addr;

                // Restore Cout Flags
                cout.flags(prevFlags);
                return;
                break;
            }


            break;



        default:
            // Output Error
            cerr << "\nUNKNOWN INSTRUCTION AT LINE[" << line << "] BYTE[" << counter << "]\n";
            cout << "Opcode: " << opcode << endl;

            // Restore Cout Flags
            cout.flags(prevFlags);
            return;
            break;
        }
        cout << '\n';
        // cout << hex << setw(2) << setfill('0') << opcode << ' ';

        counter = (counter + 1) % 16;
        if (!counter) {
            cout << '\n';
            line++;
        }
    }

    // Restore Cout Flags
    cout.flags(prevFlags);
}