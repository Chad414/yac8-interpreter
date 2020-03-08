//
// Created by chad on 3/6/20.
//

#include "../include/Display.h"


Display::Display(CHIP8* chip8) {
    // Configure CPU Settings
    cpu = chip8;

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Set OpenGL version and stencil size
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create Window
    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 256, SDL_WINDOW_OPENGL);

    // Create OpenGL Context for window
    context = SDL_GL_CreateContext(window);
}

Display::~Display() {
    // Destroy Context on exit
    SDL_GL_DeleteContext(context);
    SDL_Quit();
}

/**
 * Main Display Run Loop
 */
void Display::run() {
    // Create window event and start loop
    SDL_Event windowEvent;
    bool keyPos = true;     // False = KeyDown | True = KeyUp
    SDL_Scancode* keyCode;  // Stored Key ScanCode
    bool isCpuRun = false;  // DEBUG: Pause/Play

    // RUN LOOP
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            switch (windowEvent.type) {
            // Keyboard Event
            case SDL_KEYDOWN:
                // Key Pressed Down
                keyPos = false;

            case SDL_KEYUP:
                keyCode = &windowEvent.key.keysym.scancode;  // Store Key ScanCode

#if DISPLAY_KEY_DEBUG  // DEBUG: Output KeyPress
                std::cout << "KeyCode[ "
                          << (keyPos ? "UP" : "DOWN") << "] = "
                          << *keyCode << '\n';
#endif
#if DISPLAY_DEBUG_MODE  // DEBUG: Debug Mode Outputs
                // Debug Key
                if (!keyPos && *keyCode == SDL_SCANCODE_F1) {
                    std::cout << "=== DEBUG START ===\n";
                    cpu->regDump(std::cout);
                    cpu->keyDump(std::cout);
                    cpu->stackDump(std::cout);
                    std::cout << "=== DEBUG END ===\n\n";
                }
#endif

                // Set Key Value
                for (u_char i = 0x0; i <= 0xF; i++) {
                    if (*keyCode == keyMap[i])
                        cpu->key[i] = !keyPos;  // Set CPU's Key to Position Pressed
                }

                // DEBUG: Step by Step Run Instruction + Output to Console
                if (!keyPos && *keyCode == SDL_SCANCODE_SPACE) {
                    system("clear && date");
                    cpu->run(true);
                    cpu->displayDump(std::cout);
                    cpu->regDump(std::cout);
                    cpu->keyDump(std::cout);
                } else if (!keyPos && *keyCode == SDL_SCANCODE_RETURN) {
                    isCpuRun = !isCpuRun;
                }

                // Reset Key Pos
                keyPos = true;
                break;

            default:
                break;
            }

            // Check if close button was clicked
            if (windowEvent.type == SDL_QUIT) break;
        }

        // DEBUG: Play Run
        if (isCpuRun) {
            system("clear && date");
            cpu->run(true);
            cpu->displayDump(std::cout);
            cpu->regDump(std::cout);
            cpu->keyDump(std::cout);
        }

        // Swap front and back buffer
        SDL_GL_SwapWindow(window);
    }
}
