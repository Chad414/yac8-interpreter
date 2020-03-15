//
// Created by chad on 3/6/20.
//

#ifndef YAC8_INTERPRETER_DISPLAY_H
#define YAC8_INTERPRETER_DISPLAY_H

#define DISPLAY_KEY_DEBUG 0   // On Keypress Console Verbose
#define DISPLAY_DEBUG_MODE 1  // Debug Mode Enable (F1 Key Outputs)

#include <vector>
#include "CHIP-8.h"
#include "SimpleRender/SimpleRender.h"

GLuint InitShader(std::string, GLenum);
BufferData createBuffer(GLfloat*, size_t, GLuint*, size_t, GLuint);


class Display : SimpleRender {
  private:
    CHIP8* cpu;
    int keyMap[16] = {
        // Mapped Keys for CPU 16 Hex Keyboard
        GLFW_KEY_SPACE,       // 0x0
        GLFW_KEY_Z,           // 0x1
        GLFW_KEY_UP,          // 0x2
        GLFW_KEY_X,           // 0x3
        GLFW_KEY_LEFT,        // 0x4
        GLFW_KEY_C,           // 0x5
        GLFW_KEY_RIGHT,       // 0x6
        GLFW_KEY_V,           // 0x7
        GLFW_KEY_DOWN,        // 0x8
        GLFW_KEY_LEFT_SHIFT,  // 0x9
        GLFW_KEY_A,           // 0xA
        GLFW_KEY_S,           // 0xB
        GLFW_KEY_D,           // 0xC
        GLFW_KEY_F,           // 0xD
        GLFW_KEY_Q,           // 0xE
        GLFW_KEY_W            // 0xF
    };

  private:                                 // 2D SimpleRender Overloaded Methods
    void Preload();                        // Configure/Load Data that will be used in Application
    void Draw();                           // Main Draw location of Application
    void onKey(int, int, int, int);        // OnKey Press Method
    void onMouse(double, double){};        // On Mouse Movement
    void onMouseClick(int, int, int){};    // On Mouse Click
    void onMouseScroll(double, double){};  // On Mouse Scroll


  public:
    Display(CHIP8*);

    void run();
};



#endif  //YAC8_INTERPRETER_DISPLAY_H
