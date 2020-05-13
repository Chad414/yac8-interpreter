#pragma once

// Core Libraries
#include <stdio.h>
#include <string.h>
#include "../types.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

// SDL Libraries
#include <SDL2/SDL.h>


// Window Dimensions (CHIP8 Res = 64x32)
// #define RES_SCALE 2
#define WIDTH 64
#define HEIGHT 32


class SimpleRender {
  private:    // Private Untouchable Variables
  protected:  // Protected Variables | GL Window Data
    const u_int8_t RES_SCALE;
    double FPS;  // Current Calculated FPS Value
    uint32_t overallFrameCount = 0;

  protected:  // Shared Window Data
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    const char *title = "CHIP-8";
    char titleBuffer[256];  // Used for Temporary Character Storage (Window Title)


  protected:  // Shared Overrideable Callbacks
    virtual void onKey(SDL_KeyboardEvent &k);
    virtual void onMouseClick(SDL_MouseButtonEvent &m);
    virtual void onMouse(double xPos, double yPos);
    virtual void onMouseScroll(double xOffset, double yOffset);


  protected:  // Shared Methods
    /**
	 * Returns the Calculated Frames Per Second from Draw Loop
	 *	@returns FPS Value
	 */
    double getFPS();



  protected:  // Helper Functions
    /**
     * Draw loop
     */
    virtual void Draw();

    /**
     * Data/Properties to configure prior to Drawing
     */
    virtual void Preload();

    /* 
	 * Fixed Interval Update 
	 * Used for Physics Sync
	 *  @param deltaTime - Timestep
	 */
    virtual void fixedUpdate(double deltaTime);



  public:  // Main User-End Operation Functions
    /**
     * Constuctor that initalizes SDL
     * @param scale - Upscale Resolution
     * @param title - Window Title
     */
    SimpleRender(u_int8_t scale, const char *title = "Window");

    /**
     * Destructor to clean up Resources used by SDL
     */
    ~SimpleRender();

    /**
     * Constuctor that initalizes GLFW with default Width and Height
     */
    void InitRender();

    /**
     * Starts running OpenGL window
     * @returns - Value '-1' or '0' to determine the exit State
     */
    int run();

    /**
     * Sets the Pixel to a color at the position given
     * 
     * @param x - The X-axis position
     * @param y - The Y-axis position
     * @param color - The Color to set the pixel to
     * @param pixels - Pointer to the pixels array
     */
    static void drawPixel(int x, int y, u_int32_t color, u_int32_t *pixels);
};
