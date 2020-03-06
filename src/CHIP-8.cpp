//
// Created by chad on 3/3/20.
//
#include "../include/CHIP-8.h"

/**
 * CHIP8 Constructor
 */
CHIP8::CHIP8() {
    srand(time(NULL));  // Initialize Random Seed
    PC = 0x200;         // Set PC to ROM Starting Address in Memory
}

/**
 * Loads given ROM into Memory starting at
 *  address 0x200
 * @param romPath - File Path to ROM
 */
void CHIP8::loadROM(char* romPath) {
    std::stringstream hexStream;
    Disassembler dasm;
    dasm.hexDump(romPath, hexStream);

    u_int16_t h;
    // Store ROM in RAM starting at 0x200
    for (int addr = 0x200; hexStream >> h; addr++) {
        this->memory[addr] = h;

#if CHIP8_DEBUG == 1  // DEBUG: RAM Storage Verbose
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
const char* CHIP8::memDump() {
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
 * Outputs Register Information into given stream
 * 
 * @param out - Output Stream for Register Dump
 */
void CHIP8::regDump(std::ostream& out) {
    out << "=================== General Registers ===================\n";
    for (u_char i = 0x0; i <= 0xF; i++) {
        out << "V" << short(i) << " = 0x"
            << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
            << short(V[i]) << '\t';

        if (!((i + 1) % 4)) out << '\n';
    }

    out << "\n======== Registers ========\t";
    out << "========= Timers ========\n";
    out << "I = 0x"
        << std::uppercase << std::hex << std::setw(4) << std::setfill('0')
        << short(I) << '\t';
    out << "PC = 0x"
        << std::uppercase << std::hex << std::setw(4) << std::setfill('0')
        << short(PC) << '\t';


    out << "dT = 0x"
        << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
        << short(dTimer) << '\t';

    out << "sT = 0x"
        << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
        << short(sTimer) << '\t';

    out << std::endl;
}

/**
 * Outputs Stack Information into given stream
 * 
 * @param out - Output Stream for Stack Dump
 */
void CHIP8::stackDump(std::ostream& out) {
    out << "======== Stack ========\n";

    // Check if Emtpy
    if (stack.empty()) {
        out << "Stack = EMPTY\n";
        return;
    }

    // Save Stack
    std::stack<u_int16_t> bStack;

    // Output Entire Stack while backing up to bStack
    out << "Stack.size = " << stack.size() << '\n';
    for (int i = 0; stack.size() > 0; i++) {
        out << "Stack[" << i << "] = " << stack.top() << '\n';
        bStack.push(stack.top());
        stack.pop();
    }

    // Restore Stack
    for (int i = 0; bStack.size() > 0; i++) {
        stack.push(bStack.top());
        bStack.pop();
    }
}

/**
 * Begin running the interpreter
 * 
 * @param - isSequential - Sequential run or Infinite Loop (for threading)
 */
void CHIP8::run(bool isSequential) {
    // Loop through Memory
    do {
        // Memory[PC]    -> Left-Most Nibble
        // Memory[PC+1]  -> Right-Most Nibble
        u_int16_t opcode = (memory[PC] << 8) | memory[PC + 1];
        std::cout << "[" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
                  << PC << "] " << std::setw(2) << std::setfill('0')
                  << int(memory[PC]) << ' ' << std::setw(2) << std::setfill('0')
                  << int(memory[PC + 0x1])
                  << std::resetiosflags(std::ios::uppercase | std::ios::hex) << '\t';

        std::cout << std::hex;
        // Switch Case to handle Instructions NOTE: In Progress
        switch (memory[PC] & 0xF0) {  // Based on the First Nibble
        case 0x00:                    // System Call (SYS addr)
            // NNN = Address
            if ((opcode & 0xFFF) == 0x0E0) {  // Clear Screen (CLS)
                std::cout << "CLS";
                CLS();
            } else if ((opcode & 0xFFF) == 0x0EE) {  // Return from Subroutine (RET)
                std::cout << "RET";
                RET();
            } else {  // Output Data as in on Line
                std::cout << std::setw(4) << opcode;
                break;
            }
            break;

        case 0x10:  // Jump to Address (JP addr)
            // NNN = Address
            std::cout << "JP " << (opcode & 0xFFF);
            JP(opcode & 0xFFF);
            break;

        case 0x20:  // Calls Subroutine (CALL addr)
            // NNN = Address
            std::cout << "CALL " << (opcode & 0xFFF);
            CALL(opcode & 0xFFF);
            break;

        case 0x30:  // Skip next Instruction if(reg[x] == NN) (SE Vx, byte)
            std::cout << "SE V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant Byte
            std::cout << ", " << (opcode & 0xFF);

            SE(V[(opcode & 0xF00) >> 8], opcode & 0xFF);
            break;

        case 0x40:  // Skip next if (reg[x] != NN) (SNE Vx, byte)
            std::cout << "SNE V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant Byte
            std::cout << ", " << (opcode & 0xFF);

            SNE(V[(opcode & 0xF00) >> 8], opcode & 0xFF);
            break;

        case 0x50:  // Skip next if (reg[x] == reg[y]) (SE Vx, Vy)
            std::cout << "SE V" << ((opcode & 0xF00) >> 8);

            // Obtain next Register Byte
            std::cout << ", V" << ((opcode & 0xF0) >> 4);  // Reg[Y] -> 0x5XY0
            SE(V[(opcode & 0xF00) >> 8], V[(opcode & 0xF0) >> 4]);
            break;

        case 0x60:  // Set reg[x] = NN (LD Vx, byte)
            std::cout << "LD V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant
            std::cout << ", " << (opcode & 0xFF);
            LD(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF));
            break;

        case 0x70:  // Adds reg[x] += NN (ADD Vx, byte)
            std::cout << "ADD V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant
            std::cout << ", " << (opcode & 0xFF);

            // Add and Wraparound without Carry Flag
            ADD(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF), false);
            break;

        case 0x80:  // Register on Register Operations
            // Get Operation Type
            switch (opcode & 0xF) {  // Operation Type
            case 0x0:                // Set reg[x] = reg[y]
                std::cout << "LD V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                LD(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x1:  // Set reg[x] |= reg[y]
                std::cout << "OR V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);

                OR(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x2:  // Set reg[x] &= reg[y]
                std::cout << "AND V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);

                AND(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x3:  // Set reg[x] ^= reg[y]
                std::cout << "XOR V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);

                XOR(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x4:  // Set reg[x] += reg[y]
                std::cout << "ADD V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                ADD(&V[(opcode & 0xF00) >> 8], V[opcode & 0xFF], true);
                break;
            case 0x5:  // Set reg[x] -= reg[y]
                std::cout << "SUB V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                SUB(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x6:  // Shift reg[x] >>= 1
                std::cout << "SHR V" << ((opcode & 0xF00) >> 8);

                SHR(&V[(opcode & 0xF00) >> 8], &V[opcode & 0xF0]);
                break;

            case 0x7:  // Set reg[x] = reg[y] - reg[x]
                std::cout << "SUBN V" << ((opcode & 0xF00) >> 8);
                std::cout << ", V" << (opcode & 0xF0);
                SUBN(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;

            case 0xE:  // Shift regx[] <<= 1
                std::cout << "SHL V" << ((opcode & 0xF00) >> 8);
                SHL(&V[(opcode & 0xF00) >> 8], &V[opcode & 0xF0]);
                break;

            default:
                // Output Data as in on Line
                std::cout << std::setw(4) << opcode;
                break;
            }

            break;

        case 0x90:  // Skip next if (reg[x] != reg[y])
            std::cout << "SNE V" << ((opcode & 0xF00) >> 8);

            // Obtain next Register Byte
            std::cout << ", V" << (opcode & 0xF0);
            SNE(V[(opcode & 0xF00) >> 8], V[opcode & 0xFF]);
            break;

        case 0xA0:  // Set Register I = addr
            std::cout << "LD I, ";

            // Output Address
            std::cout << (opcode & 0xFFF);
            LD(u_int16_t(opcode & 0xFFF));
            break;

        case 0xB0:  // Jumps to location in addr + Reg[0]
            std::cout << "JP V0, ";

            // Output Address
            std::cout << (opcode & 0xFFF);
            JP((opcode & 0xFFF) + V[0x0]);
            break;

        case 0xC0:  // Sets reg[x] = byte
            std::cout << "RND V" << ((opcode & 0xF00) >> 8);

            // Get Const Byte
            std::cout << ", " << (opcode & 0xFF);
            RND(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF));
            break;

        case 0xD0:  // Draw n-byte sprite at mem (reg[x], reg[y])
            std::cout << "DRW V" << ((opcode & 0xF00) >> 8);

            // Get next Byte
            std::cout << ", V" << ((opcode & 0xF0) >> 4)  // Vy
                      << ", " << (opcode & 0x0F);         // n

            break;

        case 0xE0:  // Skip/No-Skip next Instruction if Key in reg[x] is pressed
            // Check (Skip/No-Skip)
            if ((opcode & 0xFF) == 0x9E) {  // Skip
                std::cout << "SKP V" << ((opcode & 0xF00) >> 8);
            } else {  // addr == 0xA1 (No Skip)
                std::cout << "SKNP V" << ((opcode & 0xF00) >> 8);
            }

            break;

        case 0xF0:  // Timer | Key Press | Index Register | Sprite
            // Action Type
            switch (opcode & 0xFF) {
            case 0x07:  // Delay Timer Value by DT
                std::cout << "LD V" << ((opcode & 0xF00) >> 8) << ", DT";

                LD(&V[(opcode & 0xF00) >> 8], dTimer);
                break;

            case 0x0A:  // Wait for Key Press and store Key in reg[x]
                std::cout << "LD V" << ((opcode & 0xF00) >> 8) << ", K";

                // TODO: When SDL is included
                break;

            case 0x15:  // Set Delay Timer to reg[x]
                std::cout << "LD DT, V" << ((opcode & 0xF00) >> 8);

                LD(&dTimer, V[(opcode & 0xF00) >> 8]);
                break;

            case 0x18:  // Set Sound Timer to reg[x]
                std::cout << "LD ST, V" << ((opcode & 0xF00) >> 8);

                LD(&sTimer, V[(opcode & 0xF00) >> 8]);
                break;

            case 0x1E:  // Set values of I to reg[x] I += reg[x]
                std::cout << "ADD I, V" << ((opcode & 0xF00) >> 8);

                ADD(&I, V[(opcode & 0xF00) >> 8]);
                break;

            case 0x29:  // Set I to the location of Sprite Digit reg[x]
                std::cout << "LD F, V" << ((opcode & 0xF00) >> 8);

                // TODO: After DRW is implemented
                break;

            case 0x33:  // Store BCD rep of reg[x] in mem locaion I, I+1, and I+2
                std::cout << "LD B, V" << ((opcode & 0xF00) >> 8);
                LD(V[(opcode & 0xF00) >> 8]);
                break;

            case 0x55:  // Store reg[0] to reg[x] in mem starting at location I
                std::cout << "LD [I], V" << ((opcode & 0xF00) >> 8);
                LD(&I, (opcode & 0xF00) >> 8);
                break;

            case 0x65:  // Read reg[0] to reg[x] in mem starting at location I
                std::cout << "LD V" << ((opcode & 0xF00) >> 8)
                          << ", [I]";
                LD(((opcode & 0xF00) >> 8), &I);
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
        // Go to next Line
        PC += 0x2;
    } while (!isSequential && PC < 0xFFF);  // Make sure PC stays within Memory
}

/**
 * Opcode(s): 00E0 
 * Clears the Screen
 */
void CHIP8::CLS() {
    memset(display, 0x00, 64 * 32);
}

/**
 * Opcode(s): 00EE 
 * Return from Subroutine, return
 */
void CHIP8::RET() {
    PC = stack.top();
    stack.pop();
}

/**
 * Opcode(s): 1NNN, BNNN 
 * Jump to address NNN, requires only
 *  1 Byte and 1 Nibble (0xFFF)
 * 
 * @param addr - 2 Byte Address to jump to
 */
void CHIP8::JP(u_int16_t addr) {
    PC = addr;  // Set PC to NNN
}

/**
 * Opcode(s): 2NNN 
 * Calls address NNN by setting the 
 *  PC to that Address
 * Address used: 1 Byte and 1 Nibble (0xFFF)
 * 
 * @param addr - 2 Byte Address to CALL
 */
void CHIP8::CALL(u_int16_t addr) {
    stack.push(PC);  // Push current PC to stack
    PC = addr;       // Set PC to NNN
}

/**
 * Opcode(s): 3XKK, 5XY0
 * Skip next instruction if Vx = kk
 * 
 * @param byte1 - First Byte to compare to second
 * @param byte2 - Second Byte being compared to
 */
void CHIP8::SE(u_char byte1, u_char byte2) {
    if (byte1 == byte2)
        PC += 0x2;
}

/**
 * Opcode(s): 4XKK, 9XY0
 * Skip next instruction if Vx != kk
 * 
 * @param byte1 - First Byte to compare to second
 * @param byte2 - Second Byte being compared to
 */
void CHIP8::SNE(u_char byte1, u_char byte2) {
    if (byte1 != byte2)
        PC += 0x2;
}

/**
 * Opcode(s): 6XKK, 8XY0, FX07/15/18
 * Loads value kk into Vx
 * 
 * @param regPtr - Vx Register used
 * @param byte - kk Byte to set into Vx
 */
void CHIP8::LD(u_char* regPtr, u_char byte) {
    *regPtr = byte;
}

/**
 * Opcode(s): 7XKK, 8XY4
 * Adds value kk to Vx
 * 
 * @param regPtr - Vx Register used
 * @param byte - kk Byte to set into Vx
 * @param checkFlag - Whether to check for for Overflow and set Carry Flag
 * 
 */
void CHIP8::ADD(u_char* regPtr, u_char byte, bool checkFlag) {
    if (checkFlag)
        V[0xF] = *regPtr + byte > 0xFF ? 0x1 : 0x0;
    *regPtr = (*regPtr + byte) % 0xFF;
}

/**
 * Opcode(s): 8XY1
 * Set Vx = Vx OR Vy
 * 
 * @param regPtr - Vx Register Used
 * @param byte - Byte that will be OR-ed with Vx
 */
void CHIP8::OR(u_char* regPtr, u_char byte) {
    *regPtr |= byte;
}

/**
 * Opcode(s): 8XY2
 * Set Vx = Vx AND Vy
 * 
 * @param regPtr - Vx Register Used
 * @param byte - Byte that will be AND-ed with Vx
 */
void CHIP8::AND(u_char* regPtr, u_char byte) {
    *regPtr &= byte;
}

/**
 * Opcode(s): 8XY3
 * Set Vx = Vx XOR Vy
 * 
 * @param regPtr - Vx Register Used
 * @param byte - Byte that will be XOR-ed with Vx
 */
void CHIP8::XOR(u_char* regPtr, u_char byte) {
    *regPtr ^= byte;
}

/**
 * Opcode(s): 8XY5
 * Set Vx = Vx - Vy
 * VF (Carry Flag) set if Vx > Vy
 * 
 * @param regPtr - Vx Register Used
 * @param byte - Byte that will be Subtracted by Vx
 */
void CHIP8::SUB(u_char* regPtr, u_char byte) {
    V[0xF] = (*regPtr > byte) ? 0x1 : 0x0;
    *regPtr -= byte;
}

/**
 * Opcode(s): 8XY6
 * Shift Vx 1-bit Right (90s & 00s -> Vx >>= 1)
 * Store Vy >> 2 into Vx (70s & 80s -> Vx = Vy >> 1)
 * VF = 1 if LSB is 1
 * 
 * @param regPtr1 - Vy Register storing the shifted Result
 * @param regPtr2 - Vy Register used to Shift 1 and set to Vx (70s and 80s ROMs)
 */
void CHIP8::SHR(u_char* regPtr1, u_char* regPtr2) {
    V[0xF] = (*regPtr1 & 0x1) ? 0x1 : 0x0;  // Set Carry Flag if LSB is 1

    // 70s and 80s Supported ROMs
    // *regPtr1 = *regPtr2 >> 1;

    // 90s and 00s Supported ROMs
    *regPtr1 >>= 1;
}

/**
 * Opcode(s): 8XY7 
 * Set Vx = Vy - Vx
 * VF (Carry Flag) set if NOT Borrowed
 * 
 * @param regPtr - Vx Register Used
 * @param byte - Byte being sutracted by Vx
 */
void CHIP8::SUBN(u_char* regPtr, u_char byte) {
    V[0xF] = (byte > *regPtr) ? 0x1 : 0x0;
    *regPtr = byte - *regPtr;
}

/**
 * Opcode(s): 8XYE
 * Shift Vx 1-bit Left (90s & 00s -> Vx <<= 1)
 * Store Vy << 2 into Vx (70s & 80s -> Vx = Vy << 1)
 * VF = 1 if MSB is 1
 * 
 * @param regPtr1 - Vy Register storing the shifted Result
 * @param regPtr2 - Vy Register used to Shift 1 and set to Vx (70s and 80s ROMs)
 */
void CHIP8::SHL(u_char* regPtr1, u_char* regPtr2) {
    V[0xF] = (*regPtr1 & 0x8000) ? 0x1 : 0x0;  // Set Carry Flag if MSB is 1

    // 70s and 80s Supported ROMs
    // *regPtr1 = *regPtr2 << 1;

    // 90s and 00s Supported ROMs
    *regPtr1 <<= 1;
}

/**
 * Opcode(s): ANNN
 * Set Index Register to nnn
 * I = addr
 * 
 * @param addr - 2Byte Address being set to Index Register
 */
void CHIP8::LD(u_int16_t addr) {
    I = addr;
}

/**
 * Opcode(s): CXKK
 * Generates Random Byte
 * Vx = random byte & KK
 * 
 * @param regPtr - Vx Register being Used
 * @param byte - Byte to AND from Random Generated Byte
 */
void CHIP8::RND(u_char* regPtr, u_char byte) {
    *regPtr = (rand() % 0xFF) & byte;
}


// TODO: OpenGL and SDL Dependant
/**
 * Opcode(s): DXYN
 * Display n-byte sprite at location I at (Vx, Vy) 
 * Reads n-bytes from memory starting at location I
 * VF = Collision
 * 
 * @param regPtrX - Vx Register to set Sprite at x-position
 * @param regPtrY - Vy Register to set Sprite at y-position
 * @param nBytes - n-Bytes to read from address I
 */
void CHIP8::DRW(u_char* regPtrX, u_char* regPtrY, u_char nBytes) {  // TODO: Implement SDL and OpenGL
}

/**
 * Opcode(s): EX9E
 * Skip next instruction if key with the value of Vx is pressed
 * 
 * @param keyVal - Key Value to listen
 */
void CHIP8::SKP(u_char keyVal) {  //TODO: Implement SDL Key Down
    // IF KEY_IS_DOWN(keyVal)
    // PC += 0x2;
}

/**
 * Opcode(s): EXA1
 * Skip next instruction if key with the value of Vx is not pressed
 * 
 * @param keyVal - Key Value to listen
 */
void CHIP8::SKNP(u_char keyVal) {  // TODO: Implement SDL Key Down
    // IF (!KEY_IS_DOWN(keyVal))
    // PC += 0x2;
}



/**
 * Opcode(s): FX1E
 * Add value I + Vx to I
 * 
 * @param regPtr - I Register Used
 * @param byte - Byte to increment I by
 */
void CHIP8::ADD(u_int16_t* regPtr, u_char byte) {
    *regPtr += byte;
}

/**
 * Opcode(s): FX33
 * Store BCD Representation of passed
 *  byte into I, I+1, I+2
 * 
 * @param byte - Byte stored in BCD Representation
 */
void CHIP8::LD(u_char byte) {
    memory[I] = byte / 100;
    memory[I + 0x1] = (byte / 10) % 10;
    memory[I + 0x2] = byte % 10;
}

/**
 * Opcode(s): FX55
 * Stores Registers V0 through Vx into
 *  memory starting at location I
 * 
 * @param I - Pointer to I Register
 * @param regX - Vx Register
 */
void CHIP8::LD(u_int16_t* I, u_char regX) {
    for (u_char i = 0x0; i < regX - 1; i++) {
        memory[*(I + i)] = V[i];
    }
}

/**
 * Opcode(s): FX65
 * Reads Registers V0 through Vx from
 *  memory starting at location I
 * 
 * @param I - Pointer to I Register
 * @param regX - Vx Register
 */
void CHIP8::LD(u_char regX, u_int16_t* I) {
    for (u_char i = 0x0; i < regX - 1; i++) {
        V[i] = memory[*(I + i)];
    }
}
