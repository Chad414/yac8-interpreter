//
// Created by chad on 3/3/20.
//
#include "../include/CHIP-8.h"

/**
 * Constructs CHIP8 to Default
 */
CHIP8::CHIP8() {
    this->init();
}

/**
 * CHIP8 Constructor with Ofstream Defined
 */
CHIP8::CHIP8(std::ostream* out) {
    this->out = out;
    this->init();
}

/**
 * Initiates CHIP8's Data
 */
void CHIP8::init() {
    srand(time(NULL));  // Initialize Random Seed
    PC = 0x200;         // Set PC to ROM Starting Address in Memory
    memset(V, 0x0, 0xF);

    // Clear Screen
    CLS();

    // Load in Font Set
    for (u_char i = 0; i < 0x50; i++)
        memory[i] = fontSet[i];

    // Clear Keys
    for (u_char& k : key)
        k = false;
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

#if CHIP8_DEBUG  // DEBUG: RAM Storage Verbose
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
 * Outputs Memory Dump of current
 *  memory state with 2Bytes per line
 *  into given stream
 * 
 * @parma out - Output Stream
 */
void CHIP8::memDump(std::ostream& out) {
    // Output 2 Bytes Per Line
    for (int i = 0x0; i < 0xFFF; i += 0x2) {
        out << "[0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex
            << i << "] " << std::hex << std::setw(2) << std::setfill('0')
            << short(this->memory[i]) << ' ' << std::setw(2) << std::setfill('0')
            << short(this->memory[i + 1])
            << std::resetiosflags(std::ios::hex | std::ios::uppercase) << '\n';
    }
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
 * Outputs Keyboard Key Information into given stream
 * 
 * @param out - Output Stream for Key Dump
 */
void CHIP8::keyDump(std::ostream& out) {
    out << "========== Hex Keyboard ==========\n";
    for (u_char i = 0x0; i <= 0xF; i++) {
        out << "Key[0x"
            << std::hex << std::uppercase
            << short(i) << "] = "
            << short(key[i]) << '\n';
    }
}

/**
 * Outputs the Display to a Stream
 * 
 * @param out - Stream to output Display to
 */
void CHIP8::displayDump(std::ostream& out) {
    for (u_char y = 0; y < 32; y++) {
        for (u_char x = 0; x < 64; x++) {
            out << (display[x][y] ? "▓" : "░");
        }
        out << '\n';
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
        if (out) *out << "[" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
                      << PC << "] " << std::setw(2) << std::setfill('0')
                      << int(memory[PC]) << ' ' << std::setw(2) << std::setfill('0')
                      << int(memory[PC + 0x1])
                      << std::resetiosflags(std::ios::uppercase | std::ios::hex) << '\t';

        if (out) *out << std::hex;
        switch (memory[PC] & 0xF0) {  // Based on the First Nibble
        case 0x00:                    // System Call (SYS addr)
            // NNN = Address
            if ((opcode & 0xFFF) == 0x0E0) {  // Clear Screen (CLS)
                if (out) *out << "CLS";
                CLS();
            } else if ((opcode & 0xFFF) == 0x0EE) {  // Return from Subroutine (RET)
                if (out) *out << "RET";
                RET();
            } else {  // Output Data as in on Line
                if (out) *out << std::setw(4) << opcode;
                break;
            }
            break;

        case 0x10:  // Jump to Address (JP addr)
            // NNN = Address
            if (out) *out << "JP " << (opcode & 0xFFF);
            JP(opcode & 0xFFF);

            // Decrement PC, since it's incrementing at the End
            //  which restores it Address Jumped to
            PC -= 0x02;
            break;

        case 0x20:  // Calls Subroutine (CALL addr)
            // NNN = Address
            if (out) *out << "CALL " << (opcode & 0xFFF);
            CALL(opcode & 0xFFF);
            break;

        case 0x30:  // Skip next Instruction if(reg[x] == NN) (SE Vx, byte)
            if (out) *out << "SE V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant Byte
            if (out) *out << ", " << (opcode & 0xFF);

            SE(V[(opcode & 0xF00) >> 8], opcode & 0xFF);
            break;

        case 0x40:  // Skip next if (reg[x] != NN) (SNE Vx, byte)
            if (out) *out << "SNE V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant Byte
            if (out) *out << ", " << (opcode & 0xFF);

            SNE(V[(opcode & 0xF00) >> 8], opcode & 0xFF);
            break;

        case 0x50:  // Skip next if (reg[x] == reg[y]) (SE Vx, Vy)
            if (out) *out << "SE V" << ((opcode & 0xF00) >> 8);

            // Obtain next Register Byte
            if (out) *out << ", V" << ((opcode & 0xF0) >> 4);  // Reg[Y] -> 0x5XY0
            SE(V[(opcode & 0xF00) >> 8], V[(opcode & 0xF0) >> 4]);
            break;

        case 0x60:  // Set reg[x] = NN (LD Vx, byte)
            if (out) *out << "LD V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant
            if (out) *out << ", " << (opcode & 0xFF);
            LD(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF));
            break;

        case 0x70:  // Adds reg[x] += NN (ADD Vx, byte)
            if (out) *out << "ADD V" << ((opcode & 0xF00) >> 8);

            // Obtain Constant
            if (out) *out << ", " << (opcode & 0xFF);

            // Add and Wraparound without Carry Flag
            ADD(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF), false);
            break;

        case 0x80:  // Register on Register Operations
            // Get Operation Type
            switch (opcode & 0xF) {  // Operation Type
            case 0x0:                // Set reg[x] = reg[y]
                if (out) *out << "LD V" << ((opcode & 0xF00) >> 8);
                if (out) *out << ", V" << (opcode & 0xF0);
                LD(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x1:  // Set reg[x] |= reg[y]
                if (out) *out << "OR V" << ((opcode & 0xF00) >> 8);
                if (out) *out << ", V" << (opcode & 0xF0);

                OR(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x2:  // Set reg[x] &= reg[y]
                if (out) *out << "AND V" << ((opcode & 0xF00) >> 8);
                if (out) *out << ", V" << (opcode & 0xF0);

                AND(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x3:  // Set reg[x] ^= reg[y]
                if (out) *out << "XOR V" << ((opcode & 0xF00) >> 8);
                if (out) *out << ", V" << (opcode & 0xF0);

                XOR(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x4:  // Set reg[x] += reg[y]
                if (out) *out << "ADD V" << ((opcode & 0xF00) >> 8);
                if (out) *out << ", V" << (opcode & 0xF0);
                ADD(&V[(opcode & 0xF00) >> 8], V[opcode & 0xFF], true);
                break;
            case 0x5:  // Set reg[x] -= reg[y]
                if (out) *out << "SUB V" << ((opcode & 0xF00) >> 8);
                if (out) *out << ", V" << (opcode & 0xF0);
                SUB(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;
            case 0x6:  // Shift reg[x] >>= 1
                if (out) *out << "SHR V" << ((opcode & 0xF00) >> 8);

                SHR(&V[(opcode & 0xF00) >> 8], &V[opcode & 0xF0]);
                break;

            case 0x7:  // Set reg[x] = reg[y] - reg[x]
                if (out) *out << "SUBN V" << ((opcode & 0xF00) >> 8);
                if (out) *out << ", V" << (opcode & 0xF0);
                SUBN(&V[(opcode & 0xF00) >> 8], V[opcode & 0xF0]);
                break;

            case 0xE:  // Shift regx[] <<= 1
                if (out) *out << "SHL V" << ((opcode & 0xF00) >> 8);
                SHL(&V[(opcode & 0xF00) >> 8], &V[opcode & 0xF0]);
                break;

            default:
                // Output Data as in on Line
                if (out) *out << std::setw(4) << opcode;
                break;
            }

            break;

        case 0x90:  // Skip next if (reg[x] != reg[y])
            if (out) *out << "SNE V" << ((opcode & 0xF00) >> 8);

            // Obtain next Register Byte
            if (out) *out << ", V" << (opcode & 0xF0);
            SNE(V[(opcode & 0xF00) >> 8], V[opcode & 0xFF]);
            break;

        case 0xA0:  // Set Register I = addr
            if (out) *out << "LD I, ";

            // Output Address
            if (out) *out << (opcode & 0xFFF);
            LD(u_int16_t(opcode & 0xFFF));
            break;

        case 0xB0:  // Jumps to location in addr + Reg[0]
            if (out) *out << "JP V0, ";

            // Output Address
            if (out) *out << (opcode & 0xFFF);
            JP((opcode & 0xFFF) + V[0x0]);

            // Decrement PC, since it's incrementing at the End
            //  which restores it Address Jumped to
            PC -= 0x02;

            break;

        case 0xC0:  // Sets reg[x] = byte
            if (out) *out << "RND V" << ((opcode & 0xF00) >> 8);

            // Get Const Byte
            if (out) *out << ", " << (opcode & 0xFF);
            RND(&V[(opcode & 0xF00) >> 8], (opcode & 0xFF));
            break;

        case 0xD0:  // Draw n-byte sprite at mem (reg[x], reg[y])
            if (out) *out << "DRW V" << ((opcode & 0xF00) >> 8);

            // Get next Byte
            if (out) *out << ", V" << ((opcode & 0xF0) >> 4)  // Vy
                          << ", " << (opcode & 0x0F);         // n

            DRW(&V[(opcode & 0xF00) >> 8], &V[(opcode & 0xF0) >> 4], (opcode & 0x0F));
            break;

        case 0xE0:  // Skip/No-Skip next Instruction if Key in reg[x] is pressed
            // Check (Skip/No-Skip)
            if ((opcode & 0xFF) == 0x9E) {  // Skip
                if (out) *out << "SKP V" << ((opcode & 0xF00) >> 8);
                SKP(V[((opcode & 0xF00) >> 8)]);
            } else {  // addr == 0xA1 (No Skip)
                if (out) *out << "SKNP V" << ((opcode & 0xF00) >> 8);
                SKNP(V[((opcode & 0xF00) >> 8)]);
            }

            break;

        case 0xF0:  // Timer | Key Press | Index Register | Sprite
            // Action Type
            switch (opcode & 0xFF) {
            case 0x07:  // Delay Timer Value by DT
                if (out) *out << "LD V" << ((opcode & 0xF00) >> 8) << ", DT";

                LD(&V[(opcode & 0xF00) >> 8], dTimer);
                break;

            case 0x0A:  // Wait for Key Press and store Key in reg[x]
                if (out) *out << "LD V" << ((opcode & 0xF00) >> 8) << ", K";

                // Decrement PC, since it'll increment at the end
                //  kind of "halting" PC in the same spot
                PC -= 0x2;
                SKP(V[(opcode & 0xF00) >> 8]);  // Continue IF key is pressed (restoring the PC back)
                break;

            case 0x15:  // Set Delay Timer to reg[x]
                if (out) *out << "LD DT, V" << ((opcode & 0xF00) >> 8);

                LD(&dTimer, V[(opcode & 0xF00) >> 8]);
                break;

            case 0x18:  // Set Sound Timer to reg[x]
                if (out) *out << "LD ST, V" << ((opcode & 0xF00) >> 8);

                LD(&sTimer, V[(opcode & 0xF00) >> 8]);
                break;

            case 0x1E:  // Set values of I to reg[x] I += reg[x]
                if (out) *out << "ADD I, V" << ((opcode & 0xF00) >> 8);

                ADD(&I, V[(opcode & 0xF00) >> 8]);
                break;

            case 0x29:  // Set I to the location of Sprite Digit reg[x]
                if (out) *out << "LD F, V" << ((opcode & 0xF00) >> 8);

                // Offset to the desired Hex Font Address
                // Since the Hex Fonts start at address 0x00-0x50
                //  and each Hex Font is 5Bytes, we offset by 0x5
                //  with the desired font value
                LD(u_int16_t(V[(opcode & 0xF00) >> 8] * 0x5));
                break;

            case 0x33:  // Store BCD rep of reg[x] in mem locaion I, I+1, and I+2
                if (out) *out << "LD B, V" << ((opcode & 0xF00) >> 8);
                LD(V[(opcode & 0xF00) >> 8]);
                break;

            case 0x55:  // Store reg[0] to reg[x] in mem starting at location I
                if (out) *out << "LD [I], V" << ((opcode & 0xF00) >> 8);
                LD(&I, (opcode & 0xF00) >> 8);
                break;

            case 0x65:  // Read reg[0] to reg[x] in mem starting at location I
                if (out) *out << "LD V" << ((opcode & 0xF00) >> 8)
                              << ", [I]";
                LD(((opcode & 0xF00) >> 8), &I);
                break;

            default:
                // Output Data as in on Line
                if (out) *out << std::setw(4) << opcode;
                break;
            }

            break;

        default:
            // Output Data as in on Line
            if (out) *out << std::setw(4) << opcode;
            break;
        }

        if (out) *out << '\n';
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

/**
 * Wraps index around a range
 * 
 * @param index - Index Position between [0-max_size]
 * @param max_size - Max Range to limit index to
 */
u_char wrap(u_char index, u_char max_size) {
    if (index < 0)
        return index + max_size;
    else if (index >= max_size)
        return index % max_size;
    else
        return index;
}

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
void CHIP8::DRW(u_char* regPtrX, u_char* regPtrY, u_char nBytes) {
    // Assume No Overlap
    V[0xF] = 0x0;

    for (u_int16_t i = I; i < (I + nBytes); i++) {  // Y-Coord

        // Draw Width of Sprite
        for (int bit = 0; bit < 8; bit++) {  // X-Cord
            // Get x and y Coordinates with Wapping Handled
            u_char x = wrap((*regPtrX + bit), 64);
            u_char y = wrap((*regPtrY + (i - I)), 32);

            // Get Individual Pixel
            u_char pixel = (((memory[i] << bit) & 0x80) >> 7);  //  (Backwards?)

            // Check for Overlap
            if (display[x][y] && pixel)
                V[0xF] = 0x1;

            // XOR Onto Display
            display[x][y] ^= pixel;
        }
    }
}


/**
 * Opcode(s): EX9E
 * Skip next instruction if key with the value of Vx is pressed
 * 
 * @param keyVal - Key Value to listen
 */
void CHIP8::SKP(u_char keyVal) {
    if (key[keyVal])
        PC += 0x2;
}

/**
 * Opcode(s): EXA1
 * Skip next instruction if key with the value of Vx is not pressed
 * 
 * @param keyVal - Key Value to listen
 */
void CHIP8::SKNP(u_char keyVal) {
    if (key[keyVal])
        PC += 0x2;
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
