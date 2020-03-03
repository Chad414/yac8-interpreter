//
// Created by chad on 3/3/20.
//
#include "../include/CHIP-8.h"

void CHIP8::CLS() {
    memset(display, 0x00, 64 * 32);
    /*for (unsigned char & i : display) {
        i = 0;
    }*/
}

void CHIP8::RET() {

}

void CHIP8::JP(unsigned char addr) {
    PC = addr; // Set PC to NNN
}

void CHIP8::CALL(unsigned char addr) {
    stack.push(PC); // Push current PC to stack
    PC = addr; // Set PC to NNN
}

