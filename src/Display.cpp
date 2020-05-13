#include "../include/Display.h"

/* Helper Functions */
// Map value x that is between a-b to c-d
float map(float x, float a, float b, float c, float d) {
    return (x - a) / (b - a) * (d - c) + c;
}


/**
 * Simple Helper Function to manipulate a Texture's
 *  pixels using a lambda function
 * 
 * @param texture - Pointer to the SDL_Texture to extract it's pixels
 * @param fn - Lambda Function that returns void and takes in uint32_t
 *  which is a pointer to the pixels of the Texture
 */
void manipPixels(SDL_Texture *texture, std::function<void(uint32_t *pixels)> fn) {
    // Get Texture Pixels
    void *pixels_ptr;
    int pitch;

    SDL_LockTexture(texture, nullptr, &pixels_ptr, &pitch);
    uint32_t *pixels = static_cast<uint32_t *>(pixels_ptr);

    // Handle Pixles
    fn(pixels);

    // Apply Updated Pixels & Refresh Renderer
    SDL_UnlockTexture(texture);
}

/* SimpleRender Section */



void Display::Draw() {
    // Output FPS to Window Title
	sprintf(titleBuffer, "%s [%.2f FPS]", title, getFPS());

	SDL_SetWindowTitle(window, titleBuffer);

    // Redraw ONLY if Draw Flag Flipped
    if(cpu->drawFlag) {
        // Preconfigure Rendering & Texture
        SDL_SetRenderTarget(renderer, texture);                     // Set Rendering Target to use
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);   // Set Render Draw Color (For Black Clear)
        SDL_RenderClear(renderer);                                  // Clear Renderer (Black)
        
        // Handle Pixles
        manipPixels(texture, [&](uint32_t *pixels) {
            for (int x = 0; x < 64; x++)
                for (int y = 0; y < 32; y++)
                    drawPixel(x, y, cpu->display[x][y] ? 0xFFFFFF : 0x00, pixels);
        });

        
        // Draw Texture on entire Window (Depending on Debug or Not)
        SDL_RenderCopy(renderer, texture, nullptr, isDebugMode ? &drawArea : nullptr);

        // Sets the Behind te Scenes to be viewed (DRAW CALL)
        SDL_RenderPresent(renderer);
        

        // Unswitch
        cpu->drawFlag = false;
    }


    // Draw Debug Menu on Textures
    // Keys:
    //  F1 = Step Through
    //  F2 = Loop Toggle
    if(isDebugMode) {
        SDL_RenderCopy(renderer, texture, nullptr, &drawArea);              // Draw texture on Draw Area

        // Setup Texture for all Debug Output
        manipPixels(debugTexture, [&](uint32_t *pixels) {                   // Apply Buffer to Texture
            for (int y =0; y<debugArea.h; y++)
                for(int x =0; x<debugArea.w; x++)
                    pixels[x + debugArea.w * y] = debugBuffer[x + debugArea.w * y] ? 0xAAAAAA : 0x00;
        });

        
        static TTF_Font *font = TTF_OpenFont("../res/fonts/OSD.ttf", 32);
        if(font) {
            SDL_RenderCopy(renderer, debugTexture, nullptr, &debugArea); // Draw Debug Texture Area

            // Store Backup of Area
            int h = debugArea.h;
            int w = debugArea.w;
            int x = debugArea.x;
            int y = debugArea.y;


            // Draw Title
            {
                // Offset
                debugArea.x += 12;
                debugArea.w = 256;
                debugArea.h = 16;

                // Draw
                SDL_Surface *surf = TTF_RenderText_Solid(font, "F1 (Step)  |  F2 (Loop)", {111, 111, 111, 255});
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_RenderCopy(renderer, tex, nullptr, &debugArea);
                // Free Memory
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);

                // Restore Values
                debugArea.x = x;
                debugArea.w = w;
                debugArea.h = h;
            }


            // Draw Registers
            {
                // Offset
                debugArea.x += 2;
                debugArea.y += 6;
                

                // Set Area Dimensions
                debugArea.h = 14;
                debugArea.w = 64;


                for (int i =0; i<=0xF; i++) {
                    char *textBuffer = new char[255];
                    sprintf(textBuffer, "V%X = %X", i, cpu->getRegisterVal(i));
                    SDL_Surface *surf = TTF_RenderText_Solid(font, textBuffer, {255, 255, 255, 255});
                    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

                    debugArea.y += 14;
                    SDL_RenderCopy(renderer, tex, nullptr, &debugArea);

                    // Free Memory
                    SDL_FreeSurface(surf);
                    SDL_DestroyTexture(tex);
                    delete[] textBuffer;
                }


                // Restore Area Properties
                debugArea.h = h;
                debugArea.w = w;
                debugArea.x = x;
                debugArea.y = y;
            }


            // Draw Keys
            {
                // Offset
                debugArea.x += 88;
                debugArea.y += 6;

                // Set Area Dimensions
                debugArea.h = 14;
                debugArea.w = 64;

                for (int i =0; i<=0xF; i++) {
                    char *textBuffer = new char[255];
                    sprintf(textBuffer, "K%X = %X", i, cpu->key[i]);
                    SDL_Surface *surf = TTF_RenderText_Solid(font, textBuffer, {255, 255, 255, 255});
                    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

                    debugArea.y += 14;
                    SDL_RenderCopy(renderer, tex, nullptr, &debugArea);

                    // Free Memory
                    SDL_FreeSurface(surf);
                    SDL_DestroyTexture(tex);
                    delete[] textBuffer;
                }

                // Restore Area Properties
                debugArea.h = h;
                debugArea.w = w;
                debugArea.x = x;
                debugArea.y = y;
            }
            
        } else {
            spdlog::error("Display::Draw: Font Open Failed! Switching off Debug Mode");
            isDebugMode = false;
        }

        // DRAW CALL
        SDL_RenderPresent(renderer);
    }


    // Run CHIP8
    if(isLoop || isStep) {
        cpu->run(true);
        isStep = false;
    }

    // Slow Down Draw (60FPS) TODO: Make sure this works :)
    if(SDL_GetTicks() < 1000/60) {
        std::this_thread::sleep_for(std::chrono::milliseconds( (1000 / 60) - SDL_GetTicks() ));
    }
}

void Display::Preload() {
    /* Configure SDL Properties */
	// Initialize Window, Renderer, & Texture
    //  Texture will be used to draw on
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();         // Initiate TrueType

    // Window Information
    int width = WIDTH * RES_SCALE;
    int height = HEIGHT * RES_SCALE;

    // Check if Debug Setup
    if(isDebugMode) {
        // Setup Draw Area
        drawArea.w = width;
        drawArea.h = height;

        // Expand Window for Debug Area
        debugArea.x = width;        // Move the Debug Area to the Edge of the Draw Area

        debugArea.w = 300;          // Set Size of Debug Area
        debugArea.h = height;
        
        width += debugArea.w;       // Expand Window

        // Allocate Debug Buffer Data
        debugBuffer = new u_char[debugArea.w * debugArea.h];
        memset(debugBuffer, 0x00, debugArea.w * debugArea.h);


        // Draw Borders
        const int borderThickness = 1;
        
        for(int x =0; x<debugArea.w; x++)                               // Top Border
            for(int y =0; y<borderThickness; y++)
                debugBuffer[x + y * debugArea.w] = 0x1;
        for(int x =0; x<debugArea.w; x++)                               // Bottom Border
            for(int y =debugArea.h-borderThickness; y<debugArea.h; y++)
                debugBuffer[x + y * debugArea.w] = 0x1;
        for(int x =0; x<borderThickness; x++)                           // Left Border
            for(int y =0; y<debugArea.h; y++)
                debugBuffer[x + y * debugArea.w] = 0x1;
        for(int x =debugArea.w-borderThickness; x<debugArea.w; x++)     // Right Border
            for(int y =0; y<debugArea.h; y++)
                debugBuffer[x + y * debugArea.w] = 0x1;
    }


    // Setup Window, Renderer, and Textures
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

    // Setup Debug Texture
    debugTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        debugArea.w,
        debugArea.h);
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

        // Debug Keys
        if(isDebugMode) {
            if(key.state != SDL_RELEASED) {
                if(key.keysym.sym == SDLK_F1)       isStep = true;      // Switch Step On
                else if(key.keysym.sym == SDLK_F2)  isLoop = !isLoop;   // Toggle Loop
            }
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

    // Initial Values
    isLoop = true;
    isStep = false;
}

/**
 * Clean up Data
 *  - Cleans up Debug Buffer
 */
Display::~Display() {
    // Clean up Debug Data
    delete[] debugBuffer;
    SDL_DestroyTexture(debugTexture);

    // TrueType Done :0
    TTF_Quit();
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
    this->isDebugMode = true;       // Setup Debug Mode
    this->isLoop = false;           // No Loop
}