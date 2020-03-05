//
// Created by chad on 3/3/20.
//
#include "../include/CHIP-8.h"

/**
 * CHIP8 Constructor
 */
CHIP8::CHIP8() {
    srand(time(NULL));
}

/**
 * Loads given ROM into Memory starting at
 *  address 0x200
 * @param romPath - File Path to ROM
 */
void CHIP8::loadROM(char *romPath) {
    std::stringstream hexStream;
    Disassembler dasm;
    dasm.hexDump(romPath, hexStream);

    unsigned short h;
    // Store ROM in RAM starting at 0x200
    for (int addr = 0x200; hexStream >> h; addr++) {
        this->memory[addr] = h;

#if CHIP8_DEBUG == 1 // DEBUG: RAM Storage Verbose
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << "RAM[0x" << std::uppercase << addr << "]:"
                  << std::setw(2) << std::setfill('0')
                  << short(this->memory[addr])
                  << std::resetiosflags(std::ios::hex | std::ios::uppercase) << "  ";
        if (!((addr + 1) % 8)) std::cout << '\n';
#endif
    }
}

/**
 * Returns a Memory Dump of current
 *  memory state with 2Bytes per line
 */
const char *CHIP8::memDump() {
    std::stringstream dump;

    // Output 2 Bytes Per Line
    for (int i = 0x0; i < 0xFFF; i += 0x2) {
        dump << "[0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex
             << i << "] " << std::hex << std::setw(2) << std::setfill('0')
             << short(this->memory[i]) << ' ' << std::setw(2) << std::setfill('0')
             << short(this->memory[i + 1])
             << std::resetiosflags(std::ios::hex | std::ios::uppercase) << '\n';
    }

    return dump.str().c_str();
}

/**
 * Begin running the interpreter
 */
void CHIP8::run() {
    // Initialize Register Values
    PC = 0x200;

    // Loop through Memory
    while (PC < 0xFFF) { // Make sure PC stays within Memory
        // Memory[PC]    -> Left-Most Nibble
        // Memory[PC+1]  -> Right-Most Nibble
        unsigned short opcode = (memory[PC] << 8) | memory[PC + 1];
        std::cout << "[" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
                  << PC << "] " << std::setw(2) << std::setfill('0')
                  << int(memory[PC]) << ' ' << std::setw(2) << std::setfill('0')
                  << int(memory[PC + 0x1])
                  << std::resetiosflags(std::ios::uppercase | std::ios::hex) << '\t';

        std::cout << std::hex;
        // Switch Case to handle Instructions NOTE: In Progress
        switch (memory[PC] & 0xF0) { // Based on the First Nibble
        case 0x00:                   // System Call (SYS addr)
            // NNN = Address
            if ((opcode & 0xFFF) == 0x0E0) { // Clear Screen (CLS)
                std::cout << "CLS";
                CLS();
            } else if ((opcode & 0xFFF) == 0x0EE) { // Return from Subroutine (RET)
                std::cout << "RET";
                RET();
            } else { // Output Data as in on Line
                std::cout << std::setw(4) << opcode;
                break;
            }
            break;

        case 0x10: // Jump to Address (JP addr)
            // NNN = Address
            std::cout << "JP " << (opcode & 0xFFF);
            JP(opcode & 0xFFF);
            break;

        case 0x20: // Calls Subroutine (CALL addr)
            // NNN = Address
            std::cout << "CALL " << (opcode & 0xFFF);
            CALL(opcode & 0xFFF);
            break;

        case 0x30: // Skip next Instruction if(reg[x] == NN) (SE Vx, byte)
            std::cout << "SE V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant Byte
            std::cout << ", " << (opcode & 0xFF);

            SE(V[(opcode & 0xF00) >> 8], opcode & 0xFF);
            break;

        case 0x40: // Skip next if (reg[x] != NN) (SNE Vx, byte)
            std::cout << "SNE V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant Byte
            std::cout << ", " << (opcode & 0xFF);

            SNE(V[(opcode & 0xF00) >> 8], opcode & 0xFF);
            break;

        case 0x50: // Skip next if (reg[x] == reg[y]) (SE Vx, Vy)
            std::cout << "SE V" << ((opcode & 0xF00) >> 8);

            // Obtain next Register Byte
            std::cout << ", V" << ((opcode & 0xF0) >> 4); // Reg[Y] -> 0x5XY0
            SE(V[(opcode & 0xF00) >> 8], V[(opcode & 0xF0) >> 4]);
            break;

        case 0x60: // Set reg[x] = NN (LD Vx, byte)
            std::cout << "LD V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant
            std::cout << ", " << (opcode & 0xFF);
            LD(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF));
            break;

        case 0x70: // Adds reg[x] += NN (ADD Vx, byte)
            std::cout << "ADD V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant
            std::cout << ", " << (opcode & 0xFF);

            // Add and Wraparound without Carry Flag
            ADD(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF), false);
            break;

        case 0x80: // Register on Register Operations
            // Get Operation Type
            switch (opcode & 0xF) { // Operation Type
            case 0x0:               // Set reg[x] = reg[y]
                std::cout << "LD V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                LD(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x1: // Set reg[x] |= reg[y]
                std::cout << "OR V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);

                OR(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x2: // Set reg[x] &= reg[y]
                std::cout << "AND V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);

                AND(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x3: // Set reg[x] ^= reg[y]
                std::cout << "XOR V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);

                XOR(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x4: // Set reg[x] += reg[y]
                std::cout << "ADD V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                ADD(&V[(opcode & 0xF00) >> 8], V[opcode & 0xFF], true);
                break;
            case 0x5: // Set reg[x] -= reg[y]
                std::cout << "SUB V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                SUB(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x6: // Shift reg[x] >>= 1
                std::cout << "SHR V" << ((opcode & 0xF00) >> 8);

                SHR(&V[(opcode & 0xF00) >> 8], &V[opcode & 0xF0]);
                break;

            case 0x7: // Set reg[x] = reg[y] - reg[x]
                std::cout << "SUBN V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                SUBN(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;

            case 0xE: // Shift regx[] <<= 1
                std::cout << "SHL V" << ((opcode & 0xF00) >> 8);
                SHL(&V[(opcode & 0xF00) >> 8], &V[opcode & 0xF0]);
                break;

            default:
                // Output Data as in on Line
                std::cout << std::setw(4) << opcode;
                break;
            }

            break;

        case 0x90: // Skip next if (reg[x] != reg[y])
            std::cout << "SNE V" << ((opcode & 0xF00) >> 8);

            // Obtain next Register Byte
            std::cout << ", V" << (opcode & 0xF0);
            SNE(V[(opcode & 0xF00) >> 8], V[opcode & 0xFF]);
            break;

        case 0xA0: // Set Register I = addr
            std::cout << "LD I, ";

            // Output Address
            std::cout << (opcode & 0xFFF);
            LD(opcode & 0xFFF);
            break;

        case 0xB0: // Jumps to location in addr + Reg[0]
            std::cout << "JP V0, ";

            // Output Address
            std::cout << (opcode & 0xFFF);
            JP((opcode & 0xFFF) + V[0x0]);
            break;

        case 0xC0: // Sets reg[x] = byte
            std::cout << "RND V" << ((opcode & 0xF00) >> 8);

            // Get Const Byte
            std::cout << ", " << (opcode & 0xFF);
            RND(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF));
            break;

        case 0xD0: // Draw n-byte sprite at mem (reg[x], reg[y])
            std::cout << "DRW V" << ((opcode & 0xF00) >> 8);

            // Get next Byte
            std::cout << ", V" << ((opcode & 0xF0) >> 4) // Vy
                      << ", " << (opcode & 0x0F);        // n

            break;

        case 0xE0: // Skip/No-Skip next Instruction if Key in reg[x] is pressed
            // Check (Skip/No-Skip)
            if ((opcode & 0xFF) == 0x9E) { // Skip
                std::cout << "SKP V" << ((opcode & 0xF00) >> 8);
            } else { // addr == 0xA1 (No Skip)
                std::cout << "SKNP V" << ((opcode & 0xF00) >> 8);
            }

            break;

        case 0xF0: // Timer | Key Press | Index Register | Sprite
            // Action Type
            switch (opcode & 0xFF) {
            case 0x07: // Delay Timer Value by DT
                std::cout << "LD V" << ((opcode & 0xF00) >> 8) << ", DT";
                break;

            case 0x0A: // Wait for Key Press and store Key in reg[x]
                std::cout << "LD V" << ((opcode & 0xF00) >> 8) << ", K";
                break;

            case 0x15: // Set Delay Timer to reg[x]
                std::cout << "LD DT, V" << ((opcode & 0xF00) >> 8);
                break;

            case 0x18: // Set Sound Timer to reg[x]
                std::cout << "LD ST, V" << ((opcode & 0xF00) >> 8);
                break;

            case 0x1E: // Set values of I to reg[x] I += reg[x]
                std::cout << "ADD I, V" << ((opcode & 0xF00) >> 8);
                break;

            case 0x29: // Set I to the location of Sprite Digit reg[x]
                std::cout << "LD F, V" << ((opcode & 0xF00) >> 8);
                break;

            case 0x33: // Store BCD rep of reg[x] in mem locaion I, I+1, and I+2
                std::cout << "LD B, V" << ((opcode & 0xF00) >> 8);
                break;

            case 0x55: // Store reg[0] to reg[x] in mem starting at location I
                std::cout << "LD [I], V" << ((opcode & 0xF00) >> 8);
                break;

            case 0x65: // Read reg[0] to reg[x] in mem starting at location I
                std::cout << "LD V" << ((opcode & 0xF00) >> 8)
                          << ", [I]";
                break;

            default:
                // Output Data as in on Line
                std::cout << std::setw(4) << opcode;
                break;
            }


            break;



        default:
            // Output Data as in on Line
            std::cout << std::setw(4) << opcode;
            break;
        }

        std::cout << '\n';

        if (PC >= 0x282) break;
        // Go to next Line
        PC += 0x2;
    }
}

void CHIP8::CLS() {
    memset(display, 0x00, 64 * 32);
}

void CHIP8::RET() {
    PC = stack.top();
    stack.pop();
}

void CHIP8::JP(unsigned short addr) {
    PC = addr; // Set PC to NNN
}

void CHIP8::CALL(unsigned short addr) {
    stack.push(PC); // Push current PC to stack
    PC = addr;      // Set PC to NNN
}

void CHIP8::SE(u_char byte1, u_char byte2) {
    if (byte1 == byte2)
        PC += 0x2;
}

void CHIP8::SNE(u_char byte1, u_char byte2) {
    if (byte1 != byte2)
        PC += 0x2;
}
// void CHIP8::LD(unsigned short *regPtr, unsigned short addr) {}
void CHIP8::LD(u_char *regPtr, u_char byte) {
    *regPtr = byte;
}

void CHIP8::ADD(u_char *regPtr, u_char byte, bool checkFlag) {
    if (checkFlag)
        V[0xF] = *regPtr + byte > 0xFF ? 0x1 : 0x0;
    *regPtr = (*regPtr + byte) % 0xFF;
}

void CHIP8::OR(u_char *regPtr, u_char byte) {
    *regPtr |= byte;
}

void CHIP8::AND(u_char *regPtr, u_char byte) {
    *regPtr &= byte;
}

void CHIP8::XOR(u_char *regPtr, u_char byte) {
    *regPtr ^= byte;
}

void CHIP8::SUB(u_char *regPtr, u_char byte) {
    V[0xF] = (*regPtr > byte) ? 0x1 : 0x0;
    *regPtr -= byte;
}

void CHIP8::SHR(u_char *regPtr1, u_char *regPtr2) {
    V[0xF] = (*regPtr1 & 0x1) ? 0x1 : 0x0; // Set Carry Flag if LSB is 1
    *regPtr1 >>= 1;
}

void CHIP8::SUBN(u_char *regPtr, u_char byte) {
    V[0xF] = (byte > *regPtr) ? 0x1 : 0x0;
    *regPtr = byte - *regPtr;
}

void CHIP8::SHL(u_char *regPtr1, u_char *regPtr2) {
    V[0xF] = (*regPtr1 & 0x8000) ? 0x1 : 0x0; // Set Carry Flag if MSB is 1
    *regPtr1 <<= 1;
}

void CHIP8::LD(unsigned short addr) {
    I = addr;
}

void CHIP8::RND(u_char *regPtr, u_char byte) {
    *regPtr = (rand() % 0xFF) & byte;
}
