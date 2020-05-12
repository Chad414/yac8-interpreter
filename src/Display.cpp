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

    // Redraw ONLY if Draw Flag Flipped
    if(cpu->drawFlag) {
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

        // Unswitch
        cpu->drawFlag = false;
    }


    // Draw Debug Menu on Textures


    // Run CHIP8
    cpu->run(true);
    //cpu->displayDump(std::cout);
    // cpu->regDump(std::cout);
    // cpu->keyDump(std::cout);

    // Slow Down Draw (60FPS)
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

void Display::Preload() {
    /* Configure SDL Properties */
	// Initialize Window, Renderer, & Texture
    //  Texture will be used to draw on
    SDL_Init(SDL_INIT_VIDEO);

    // Window Information
    int width = WIDTH * RES_SCALE;
    int height = HEIGHT * RES_SCALE;

    // Check if Debug Setup
    if(isDebugMode) {
        // Change Rendering for Debug Area
    }


    window = SDL_CreateWindow(
        "yagb_emu",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT);

    
}

/**
 * Handle Key Press on CHIP8 Key Map
 */
void Display::onKey(SDL_KeyboardEvent &key) {
	// printf("KEY: Key[%d], ScanCode[%d], State[%d], Mod[%d]\n", k.keysym.sym, k.keysym.scancode, k.state, k.keysym.mod);
    
    // Adjust Transformation
    if (key.state == SDL_PRESSED || key.state == SDL_RELEASED) {
        // Set Key Value
        for (u_char i = 0x0; i <= 0xF; i++) {
            if (key.keysym.sym == keyMap[i])
                cpu->key[i] = (key.state == SDL_PRESSED);  // Set CPU's Key to Position Pressed
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


/**
 * Enables Debug Mode
 */
void Display::enableDebugMode() {
    this->isDebugMode = true;
}