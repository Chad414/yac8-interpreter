//
// Created by chad on 3/6/20.
//

#ifndef YAC8_INTERPRETER_DISPLAY_H
#define YAC8_INTERPRETER_DISPLAY_H

#define DISPLAY_KEY_DEBUG 0   // On Keypress Console Verbose
#define DISPLAY_DEBUG_MODE 1  // Debug Mode Enable (F1 Key Outputs)

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include "../include/CHIP-8.h"
#include "../include/BufferData.h"

GLuint InitShader(std::string, GLenum);
BufferData createBuffer(GLfloat*, size_t, GLuint*, size_t, GLuint);


struct Shader {
    bool status;  // Keep track of Shader Status (False = Not Ready | True = Ready)
    GLuint ID;    // Store Compiled Shader Program

    Shader() : ID(0), status(false){};             // No Shader Given
    Shader(GLuint _id) : ID(_id), status(true){};  // Initialize Shader to precompiled Program

    // Uses Current Program (If any)
    void use();

    // Compiles Given Shader Files (Vertex, Fragment)
    void compile(const char*, const char*);
};

class Display {
  private:
    CHIP8* cpu;
    SDL_Window* window;
    SDL_GLContext context;
    SDL_Scancode keyMap[16] = {
        // Mapped Keys for CPU 16 Hex Keyboard
        SDL_SCANCODE_SPACE,   // 0x0
        SDL_SCANCODE_Z,       // 0x1
        SDL_SCANCODE_UP,      // 0x2
        SDL_SCANCODE_X,       // 0x3
        SDL_SCANCODE_LEFT,    // 0x4
        SDL_SCANCODE_C,       // 0x5
        SDL_SCANCODE_RIGHT,   // 0x6
        SDL_SCANCODE_V,       // 0x7
        SDL_SCANCODE_DOWN,    // 0x8
        SDL_SCANCODE_LSHIFT,  // 0x9
        SDL_SCANCODE_A,       // 0xA
        SDL_SCANCODE_S,       // 0xB
        SDL_SCANCODE_D,       // 0xC
        SDL_SCANCODE_F,       // 0xD
        SDL_SCANCODE_Q,       // 0xE
        SDL_SCANCODE_W        // 0xF
    };

  private:                               // OpenGL Private Section
    std::vector<BufferData> bufferData;  // Store References the Buffer Data
    Shader shader;
    void Preload();
    void Draw();


  public:
    Display(CHIP8*);
    ~Display();
    void run();
};



#endif  //YAC8_INTERPRETER_DISPLAY_H
