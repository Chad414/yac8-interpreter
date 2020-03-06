//
// Created by chad on 3/6/20.
//

#ifndef YAC8_INTERPRETER_DISPLAY_H
#define YAC8_INTERPRETER_DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "../include/CHIP-8.h"

class Display {
  private:
    CHIP8* cpu;
    SDL_Window* window;
    SDL_GLContext context;

  public:
    Display(CHIP8*);
    ~Display();
    void run();

};



#endif //YAC8_INTERPRETER_DISPLAY_H
