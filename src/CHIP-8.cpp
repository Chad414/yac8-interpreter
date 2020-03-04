//
// Created by chad on 3/3/20.
//
#include "../include/CHIP-8.h"
#include <sstream>
#include "../include/Disassembler.h"

/**
 * Loads given ROM into Memory starting at
 *  address 0x200
 * @param romPath - File Path to ROM
 */
void CHIP8::LOAD(char* romPath) {
    std::stringstream hexStream;
    Disassembler dasm;
    dasm.hexDump(romPath, hexStream);

    unsigned short h;
    // Store ROM in RAM starting at 0x200
    for (int addr = 200; hexStream >> h; addr++) {
        this->memory[addr] = h;

#if CHIP8_DEBUG == 1  // DEBUG: RAM Storage Verbose
        std::cout << "RAM[" << addr << "]:" << std::hex << std::setw(2) << std::setfill('0') << short(this->memory[addr])
                  << std::resetiosflags(std::ios::hex) << "  ";
        if (!((addr + 1) % 8)) std::cout << '\n';
#endif
    }
}

/**
 * Returns a Memory Dump of current
 *  memory state with 2Bytes per line
 */
const char* CHIP8::memDump() {
    std::stringstream dump;

    // Output 2 Bytes Per Line
    for (int i = 0; i < 4096; i += 2) {
        dump << "[" << std::setw(4) << std::setfill('0')
                  << i << "] " << std::hex << std::setw(2) << std::setfill('0')
                  << short(this->memory[i]) << ' ' << std::setw(2) << std::setfill('0')
                  << short(this->memory[i + 1])
                  << std::resetiosflags(std::ios::hex) << '\n';
    }

    return dump.str().c_str();
}


void CHIP8::CLS() {
    memset(display, 0x00, 64 * 32);
    /*for (unsigned char & i : display) {
        i = 0;
    }*/
}

void CHIP8::RET() {
}

void CHIP8::JP(unsigned char addr) {
    PC = addr;  // Set PC to NNN
}

void CHIP8::CALL(unsigned char addr) {
    stack.push(PC);  // Push current PC to stack
    PC = addr;       // Set PC to NNN
}
