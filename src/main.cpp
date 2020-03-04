#include <iostream>
#include <string>

#include "../include/Disassembler.h"
#include "../include/CHIP-8.h"

using namespace std;

int main(int argc, char** argv) {
    // Argument Variables
    char *romPath, *asmOutput;
    bool isDisassemble = false;

    // Check Arguments
    for (int i = 0; i < argc; ++i) {
        string arg = argv[i];  // For Comparison

        // Check for Help Argument
        if (arg == "-h") {
            cout << "Usage: yac8 [romPath] {asmOutput} [OPTIONS]\n\n"
                 << "INFO:\n"
                 << "romPath \t Path to ROM that will be used\n"
                 << "asmOutput \t Optional Parameter for outputting ASM Code to File\n\n"

                 << "OPTIONS:\n"
                 << "-h \t\t Outputs Help Manual\n"
                 << "-d \t\t Disassemble Given Rom\n";
            exit(0);
        } else if (arg == "-d") {
            isDisassemble = true;  // Disassemble and Output
        }

        // Check for Path Options
        else {
            if (i == 1)
                romPath = argv[i];
            else if (i == 2)
                asmOutput = argv[i];
        }
    }

    // Check if Rom was Given
    if (romPath == NULL) {
        cerr << "No ROM Path Given!\n";
        exit(1);
    }
    std::cout << romPath << std::endl;

    // Disassemble Option
    auto* dis = new Disassembler();
    if (isDisassemble) {
        if (asmOutput == NULL)
            dis->disassemble(romPath, cout);  // Output to Console
        else {                                // Output to File
            cout << "Saving ASM to '" << asmOutput << "'\n";
            ofstream file(asmOutput);
            dis->disassemble(romPath, file);
            file.close();
        }
    }


    // CHIP-8 Run NOTE: In Progress
    CHIP8 cpu;
    cpu.loadROM(romPath);
    cpu.run();
    exit(0);


    // DEBUG: Hex Dump for Visual
    // Hex Dump
    cout << "\n\n\nHEX DUMP" << endl;
    dis->hexDump(romPath, cout);

    // Free up Mem
    delete dis;
    return 0;
}