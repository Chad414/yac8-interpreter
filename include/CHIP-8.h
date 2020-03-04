#pragma once
#include <cstring>
#include <stack>

#define CHIP8_DEBUG 0

/**
 * MEMORY:
 *  - 4KB Total Memory (0x000 - 0xFFF)
 *      - 512  Bytes (0x000 - 0x1FF) = CHIP-8 Interpreter (ROM)
 *          - Commonly Stored outside of Memory so, can be used
 *              for Front Data Storage
 *      - 3328 Bytes (0x200 - 0xEFF) = Free Memory (RAM)
 *      - 255  Bytes (0xF00 - 0xFFF) = Display Memory (RAM)
 * 
 * REGISTER:
 *  - 16 8-Bit Registers (V0 - VF)
 *      - VF = Carry Flag and No Borrow Flag
 *          - Subtraction = No Borrow Flag
 *          - Pixel Collision
 * 
 * INPUT:
 *  - 16 Keys (Range 0-F)
 *      - Keys { 8,4,6,2 } are Directional Inputs
 */

class CHIP8 {
  private:
    unsigned char memory[4096];        // 4K Bytes (0x000 - 0xFFF)
    unsigned char V[16];               // 16 8-bit Registers (V0 - VF)
    unsigned short PC = 0;             // Program counter
    std::stack<unsigned short> stack;  // Store return addresses when subroutines are called
    unsigned char dTimer;              // Delay Timer 60Hz (Count down from 60 to 0)
    unsigned char sTimer;              // Sound timer 60Hz (Count down from 60 to 0)
    unsigned char key[16];             // 16 Key Hex Keyboard (Key ranges from 0-F)
    unsigned char display[64 * 32];    // Graphics are Monochrome 64x32 Pixels

  public:
    void loadROM(char* romFile);  // Loads ROM Data into RAM
    void run();                   // Runs Interpreter
    const char* memDump();        // Returns a Memory Dump

    void CLS();                     // 00E0 Clears the Screen
    void RET();                     // 00EE Return from Subroutine, return;
    void JP(unsigned char addr);    // 1NNN Jump to address NNN
    void CALL(unsigned char addr);  // 2NNN Jump to address NNN
};