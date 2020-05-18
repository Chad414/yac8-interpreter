#include <iostream>
#include <string>

#include "../include/CHIP-8.h"
#include "../include/Disassembler.h"
#include "../include/Display.h"
#include "../include/types.h"

#define DEFAULT_DRAW_SCALE 8;

using namespace std;

int main(int argc, char **argv) {
    // Argument Variables
    char *romPath = NULL;
    char *asmOutput = NULL;
    bool isDisassemble = false;
    bool isDebug = false;
    int USER_DEFINED_DRAW_SPEED = -1;
    int USER_DEFINED_DRAW_SCALE = DEFAULT_DRAW_SCALE;

    // Check Arguments
    for (int i = 0; i < argc; ++i) {
        string arg = argv[i];  // For Comparison

        // Check for Help Argument
        if (arg == "-h" || arg =="--help") {
            cout << "Usage: yac8 [romPath] {asmOutput} [OPTIONS]\n\n"
                 << "INFO:\n"
                 << "romPath \t\t Path to ROM that will be used\n"
                 << "asmOutput \t\t Optional Parameter for outputting ASM Code to File\n\n"

                 << "OPTIONS:\n"
                 << "-h, --help \t\t Outputs Help Manual\n"
                 << "-d \t\t\t Disassemble Given Rom\n"
                 << "--debug \t\t Enables Debug Mode\n"
                 << "--scale [scaleVal] \t Sets Scale Value\n"
                 << "--speed [speedVal] \t Sets Speed Value\n";
            exit(0);
        } 
        else if (arg == "-d") {                         // Disassemble and Output
            isDisassemble = true;       
        } 
        else if (arg == "--debug") {                    // Debug Mode
            // Output Information about Keybinds
            std::cout << "Debug Mode Keybinds:\n"
                      << "\t - [F1]=Step Through Instructions\n"
                      << "\t - [F2]=Toggle Running Through Instructions\n"
                      << "\t - [F3]=Prompt a Memory Dump to 'memory.dump'\n";
                      

            // Enable Debugging
            isDebug = true;
        } 
        else if (arg == "--speed" && (i+1) < argc) {    // User Defined Draw Speed
            USER_DEFINED_DRAW_SPEED = stoi(argv[i+1]);
            i++;
        }
        else if (arg == "--scale" && (i+1) < argc) {    // User Defined Draw Scale
            USER_DEFINED_DRAW_SCALE = stoi(argv[i+1]);
            i++;

            // Validate Scale | Default if Invalid
            if(USER_DEFINED_DRAW_SCALE <= 0)
                USER_DEFINED_DRAW_SCALE = DEFAULT_DRAW_SCALE;
        }
        else {                                          // Check for Path Options
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
    Disassembler dasm;
    if (isDisassemble) {
        if (asmOutput == NULL)
            dasm.disassemble(romPath, cout);  // Output to Console
        else {                                // Output to File
            cout << "Saving ASM to '" << asmOutput << "'\n";
            ofstream file(asmOutput);
            dasm.disassemble(romPath, file);
            file.close();
        }
        exit(0);
    }

    // CHIP-8 Run
    CHIP8 cpu;
    Display display(&cpu, USER_DEFINED_DRAW_SCALE); // Setup Display with Scale
    display.setDrawRate(USER_DEFINED_DRAW_SPEED);   // Set Draw Rate | Default if none given
    cpu.loadROM(romPath);

    // Check to turn on Debug Mode
    if (isDebug) {
        display.enableDebugMode();
    }

    display.run();
    return 0;
}