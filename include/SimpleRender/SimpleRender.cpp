#include "SimpleRender.h"

/**
 * Sets the Pixel to a color at the position given
 *  Draws pixel for the Main Draw Pixels
 * 
 * @param x - The X-axis position
 * @param y - The Y-axis position
 * @param color - The Color to set the pixel to
 * @param pixels - Pointer to the pixels array
 */
void SimpleRender::drawPixel(int x, int y, u_int32_t color, u_int32_t *pixels) {
    // Map the Pixels
    pixels[x + (y * WIDTH)] = color;
}

/**
 ***********************************************************
 * Private Static Methods and Callbacks
 *  - Key Presses, Mouse Button, Cursor Movements, and
 *      Mouse Scrolling Callbacks
 ***********************************************************
 */

void SimpleRender::onKey(SDL_KeyboardEvent &k) {
    // Output Key Pressed
    printf("KEY: Key[%d], ScanCode[%d], State[%d], Mod[%d]\n", k.keysym.sym, k.keysym.scancode, k.state, k.keysym.mod);
}

void SimpleRender::onMouseClick(SDL_MouseButtonEvent &m) {
    // Output Key Pressed
    printf("MOUSE: Button[%d], State[%d], Clicks[%d], Coord[%d,%d]\n", m.button, m.state, m.clicks, m.x, m.y);
}

void SimpleRender::onMouse(double xPos, double yPos) {
    // Output Mouse Cursor Position
    printf("CURSOR: X[%.2f], Y[%.2f]\n", xPos, yPos);
}

void SimpleRender::onMouseScroll(double xOffset, double yOffset) {
    // Output Mouse Cursor Position
    printf("SCROLL: X-off[%.2f], Y-off[%.2f]\n", xOffset, yOffset);
}

/**
 ***********************************************************
 * Private Static Methods Backend
 * Initialization of Backend Functionallity
 ***********************************************************
 */

double SimpleRender::getFPS() {
    return FPS;
}


/**
 ***********************************************************
 * Private Helper Functions
 *
 *  - Draw Loop
 *  - Preload Function
 *      - Is Called prior to Draw
 *  - Process Input Function
 *      - Handles User Input from window
 *      - Function is Called Prior to Draw Loop
 *  - Fixed Update
 *      - Physics / Calculation Fixed Step Update
 ***********************************************************
 */

void SimpleRender::Draw() {
    // Output FPS to Window Title
    sprintf(titleBuffer, "%s [%.2f FPS]", title, getFPS());

    SDL_SetWindowTitle(window, titleBuffer);


    // Get Texture Pixels
    void *pixels_ptr;
    int pitch;

    SDL_LockTexture(texture, nullptr, &pixels_ptr, &pitch);
    uint32_t *pixels = static_cast<uint32_t *>(pixels_ptr);


    // Handle Pixles
    // drawPixel(0, 0, 0xFF0000, pixels);

    // Apply Updated Pixels & Refresh Renderer
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void SimpleRender::Preload() {
    this->InitRender();
}

void SimpleRender::fixedUpdate(double deltaTime) {}



/**
 ***********************************************************
 * Main User-End Methods
 *
 *  - Constructor for Creating the Object :)
 *  - Destructor for Cleaning Up
 *  - Inizializing & Starting the Engine once the User
 *      is ready!
 ***********************************************************
 */

SimpleRender::SimpleRender(u_int8_t scale, const char *title) : RES_SCALE(scale) {
    this->title = title;
}

SimpleRender::~SimpleRender() {
    printf("\nExiting, cleaning up first...\n");

    /* Destroy Resources */
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * Initiates Default Rendering Settings
 */
void SimpleRender::InitRender() {
    /* Configure SDL Properties */
    // Initialize Window, Renderer, & Texture
    //  Texture will be used to draw on
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "yagb_emu",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH * RES_SCALE,
        HEIGHT * RES_SCALE,
        SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT);
}

int SimpleRender::run() {
    /* Keep track of FPS & Fixed Upate */
    u_int32_t lastTime = SDL_GetTicks();
    int frameCount = 0;

    /* Run Pre-Start Function */
    Preload();


    /* Keep Window open until 'Q' key is pressed */
    SDL_Event windowEvent;
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            // Check if close button was clicked
            if (windowEvent.type == SDL_QUIT) return 0;

            switch (windowEvent.type) {
            // Handle Key Presses
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                onKey(windowEvent.key);
                break;

            // Handle Mouse Button
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                onMouseClick(windowEvent.button);
                break;

            case SDL_MOUSEMOTION:
                onMouse(windowEvent.motion.x, windowEvent.motion.y);
                break;

            case SDL_MOUSEWHEEL:
                onMouseScroll(windowEvent.wheel.x, windowEvent.wheel.y);
                break;

            default:
                break;
            }
        }

        // Measure the Speed (FPS)
        u_int32_t currentTime = SDL_GetTicks();
        frameCount++;
        if (currentTime - lastTime >= 1000) {  // 1 Second Elapsed
            FPS = frameCount;
            frameCount = 0;
            lastTime += 1000;
        }

        // Draw Here...
        Draw();

        // Keep Track of Overall FrameCount
        overallFrameCount++;
    }

    // No Issues
    return 0;
}
