#ifndef YAC8_INTERPRETER_DISPLAY_H
#define YAC8_INTERPRETER_DISPLAY_H

#define DISPLAY_KEY_DEBUG 0   // On Keypress Console Verbose
#define DISPLAY_DEBUG_MODE 1  // Debug Mode Enable (F1 Key Outputs)

#include <spdlog/spdlog.h>

#include <deque>
#include <functional>
#include <thread>
#include <vector>

#include "CHIP-8.h"
#include "SimpleRender/SimpleRender.h"
#include <SDL2/SDL_ttf.h>

class Display : SimpleRender {
  private:    // Debug Menu Configuration
    bool isDebugMode = false;                  // Enables Debug Options
    bool isLoop, isStep;                       // Steps through or loops Through CPU Run
    std::stringstream *out;                    // Used to store Output Stream from CPU
    std::deque<std::string> instructionWindow; // Window of Previous 10 Instructions
    u_char *debugBuffer;                       // Debug Buffer Screen Area (Used for Borders)
    SDL_Texture *debugTexture;                 // Texture to use on Debug Area

    SDL_Rect debugArea, instrArea, drawArea;   // Split up Draw and Debug Areas

  private:
    CHIP8 *cpu;
    int keyMap[16] = {
        // Mapped Keys for CPU 16 Hex Keyboard
        SDLK_SPACE,   // 0x0
        SDLK_z,       // 0x1
        SDLK_UP,      // 0x2
        SDLK_x,       // 0x3
        SDLK_LEFT,    // 0x4
        SDLK_c,       // 0x5
        SDLK_RIGHT,   // 0x6
        SDLK_v,       // 0x7
        SDLK_DOWN,    // 0x8
        SDLK_LSHIFT,  // 0x9
        SDLK_a,       // 0xA
        SDLK_s,       // 0xB
        SDLK_d,       // 0xC
        SDLK_f,       // 0xD
        SDLK_q,       // 0xE
        SDLK_w        // 0xF
    };

  private:                                        // 2D SimpleRender Overloaded Methods
    void Draw();                                  // Main Draw location of Application
    void Preload();                               // Overrided Preload, initiate Display
    void onKey(SDL_KeyboardEvent &);              // OnKey Press Method
    void onMouse(double, double){};               // On Mouse Movement
    void onMouseClick(SDL_MouseButtonEvent &){};  // On Mouse Click
    void onMouseScroll(double, double){};         // On Mouse Scroll


  public:
    Display(CHIP8 *, u_int8_t upscale);
    ~Display();

    void enableDebugMode();  // Enables Debug Mode
    void run();
};



#endif  //YAC8_INTERPRETER_DISPLAY_H
