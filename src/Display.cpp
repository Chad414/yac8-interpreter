//
// Created by chad on 3/6/20.
//

#include "../include/Display.h"

#include <utility>

Display::Display(CHIP8 chip8) {
    cpu = std::move(chip8);
}

void Display::run() {
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
}

