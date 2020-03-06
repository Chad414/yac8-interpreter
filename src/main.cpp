#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "../include/CHIP-8.h"
#include "../include/Disassembler.h"

using namespace std;

int main(int argc, char **argv) {
    // Argument Variables
    char *romPath = NULL;
    char *asmOutput = NULL;
    bool isDisassemble = false;

    // Check Arguments
    for (int i = 0; i < argc; ++i) {
        string arg = argv[i]; // For Comparison

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
            isDisassemble = true; // Disassemble and Output
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
    Disassembler dasm;
    if (isDisassemble) {
        if (asmOutput == NULL)
            dasm.disassemble(romPath, cout); // Output to Console
        else {                               // Output to File
            cout << "Saving ASM to '" << asmOutput << "'\n";
            ofstream file(asmOutput);
            dasm.disassemble(romPath, file);
            file.close();
        }
    }


    // CHIP-8 Run NOTE: In Progress
    CHIP8 cpu;
    cpu.loadROM(romPath);
    cpu.run();

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Set OpenGL version and stencil size
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create Window
    SDL_Window* window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 256, SDL_WINDOW_OPENGL);

    // Create OpenGL Context for window
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // Create window event and start loop
    SDL_Event windowEvent;
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {

            // Check if close button was clicked
            if (windowEvent.type == SDL_QUIT) break;

        }

        // Swap front and back buffer
        SDL_GL_SwapWindow(window);
    }

    // Destroy Context on exit
    SDL_GL_DeleteContext(context);
    SDL_Quit();

    exit(0);
    // DEBUG: Hex Dump for Visual
    // Hex Dump
    cout << "\n\n\nHEX DUMP" << endl;
    dasm.hexDump(romPath, cout);

    return 0;
}