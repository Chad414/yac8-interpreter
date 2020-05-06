#include "../include/Display.h"

/* Helper Functions */
// Map value x that is between a-b to c-d
float map(float x, float a, float b, float c, float d) {
    return (x - a) / (b - a) * (d - c) + c;
}

/* SimpleRender Section */


void Display::Draw() {
    // Output FPS to Window Title
	sprintf(titleBuffer, "%s [%.2f FPS]", title, getFPS());

	SDL_SetWindowTitle(window, titleBuffer);


	// Get Texture Pixels
	void *pixels_ptr;
	int pitch;

	SDL_LockTexture(texture, nullptr, &pixels_ptr, &pitch);
	uint32_t *pixels = static_cast<uint32_t *>(pixels_ptr);


	// Handle Pixles
    for (int x = 0; x < 64; x++)
        for (int y = 0; y < 32; y++)
            drawPixel(x, y, cpu->display[x][y] ? 0xFFFFFF : 0x00, pixels);

    // Apply Updated Pixels & Refresh Renderer
    SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);


    // Run CHIP8
    cpu->run(true);
    //cpu->displayDump(std::cout);
    // cpu->regDump(std::cout);
    // cpu->keyDump(std::cout);

}

/**
 * Handle Key Press on CHIP8 Key Map
 */
void Display::onKey(int key, int scancode, int action, int mods) {
    // Adjust Transformation
    if (action == SDL_PRESSED || action == SDL_RELEASED) {
        // Set Key Value
        for (u_char i = 0x0; i <= 0xF; i++) {
            if (key == keyMap[i])
                cpu->key[i] = (action == SDL_PRESSED);  // Set CPU's Key to Position Pressed
        }
    }
}


/* CHIP8 Section */
/**
 * Display Section
 */
Display::Display(CHIP8 *chip8, u_int8_t upscale) : SimpleRender(upscale, "YAC8") {
    // Configure CPU Settings
    cpu = chip8;
}


/**
 * Main Display Run Loop
 */
void Display::run() {
    int status = SimpleRender::run();
    if (status != 0)
        std::cerr << "Status = " << status << std::endl;
}