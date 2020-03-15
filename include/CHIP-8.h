#pragma once
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <sstream>
#include <stack>
#include "Disassembler.h"

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
// Built-in Fonts Stored in 0x00 - 0x50 for (0-F)
const u_char fontSet[0x50] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

class CHIP8 {
  private:                        // Private Variables
    u_char memory[4096];          // 4K Bytes (0x000 - 0xFFF)
    u_char V[16];                 // 16 8-bit Registers (V0 - VF)
    u_int16_t I;                  // Index Register (Memory Addresses)
    u_int16_t PC = 0x200;         // Program counter
    std::stack<u_int16_t> stack;  // Store return addresses when subroutines are called
    u_char dTimer;                // Delay Timer 60Hz (Count down from 60 to 0)
    u_char sTimer;                // Sound timer 60Hz (Count down from 60 to 0)
    std::ostream *out;            // Output Stream for Outputting Execution Instruciton Information

  private:        // Private Methods
    void init();  // Initiates CHIP8 Data

  public:                    // Public Variables
    u_char display[64][32];  // Graphics are Monochrome 64x32 Pixels
    u_char key[16];          // 16 Key Hex Keyboard (Key ranges from 0-F) | Set as True(0x1) or False(0x0)

  public:                              // Public Methods
    CHIP8();                           // Constructs CHIP8
    CHIP8(std::ostream *);             // Constructs CHIP8 with Output Stream
    void loadROM(char *romFile);       // Loads ROM Data into RAM
    void run(bool);                    // Runs Interpreter Sequentially or Infinitely
    void memDump(std::ostream &);      // Returns a Memory Dump
    void regDump(std::ostream &);      // Outputs Register Dump to Output Stream
    void stackDump(std::ostream &);    // Outputs Stack Dump to Output Stream
    void keyDump(std::ostream &);      // Dumps 16 Key Keyboard Bytes
    void displayDump(std::ostream &);  // Dumps Display to Stream

    void CLS();                            // 00E0 Clears the Screen
    void RET();                            // 00EE Return from Subroutine, return;
    void JP(u_int16_t);                    // 1NNN, BNNN Jump to address NNN
    void CALL(u_int16_t);                  // 2NNN Call address NNN
    void SE(u_char, u_char);               // 3XKK, 5XY0 Skip next instruction if Vx = kk
    void SNE(u_char, u_char);              // 4XKK, 9XY0 Skip next instruction if Vx != kk
    void LD(u_char *, u_char);             // 6XKK, 8XY0, FX07/15/18 Load value kk into Vx
    void ADD(u_char *, u_char, bool);      // 7XKK, 8XY4 Add value kk to Vx
    void OR(u_char *, u_char);             // 8XY1 Set Vx = Vx or Vy
    void AND(u_char *, u_char);            // 8XY2 Set Vx = Vx and Vy
    void XOR(u_char *, u_char);            // 8XY3 Set Vx = Vx xor Vy
    void SUB(u_char *, u_char);            // 8XY5 Set Vx = Vx - Vy | VF = NOT BORROWED
    void SHR(u_char *, u_char *);          // 8XY6 Shift Vx 1-bit Right | VF = 1 if LSB is 1
    void SUBN(u_char *, u_char);           // 8XY7 Set Vx = Vy - Vx | VF Handled
    void SHL(u_char *, u_char *);          // 8XYE Shift Vx 1-bit Left | VF = 1 if MSB is 1
    void LD(u_int16_t);                    // ANNN, FX29, Set Index Register to nnn | I = addr
    void RND(u_char *, u_char);            // CXKK, Generate Random Byte | Vx = random byte & KK
    void DRW(u_char *, u_char *, u_char);  // DXYN, Display n-byte sprite at location I at (Vx, Vy) | VF = Collision
    void SKP(u_char);                      // EX9E, SKP, Skip next instruction if key with the value of Vx is pressed
    void SKNP(u_char);                     // EXA1, SKNP, Skip next instruction if key with the value of Vx is not pressed
    void ADD(u_int16_t *, u_char);         // FX1E, Add value I + Vx to I
    void LD(u_char);                       // FX33, Store BCD Representation of VX into I, I+1, I+2
    void LD(u_int16_t *, u_char);          // FX55, Store Values V0 - VX into Memory Starting at Location I
    void LD(u_char, u_int16_t *);          // FX65, Read Values V0 - VX from Memory Starting at Location I
};
