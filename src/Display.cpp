//
// Created by chad on 3/6/20.
//

#include "../include/Display.h"

/* OpenGL Section */
/**
 * Shader Structure for Easy Shader Use
 *  and set
 */
// Uses Current Program (If any)
void Shader::use() {
    if (status)
        glUseProgram(ID);  // Make Sure ther IS a Valid Program ID
    else
        std::cerr << "Shader Struct: No Program to use!\n";
}

// Compiles Given Shader Files (Vertex, Fragment)
void Shader::compile(const char *vertFilePath, const char *fragFilePath) {
    // Initialize the Shaders
    GLuint fragShader = InitShader(fragFilePath, GL_FRAGMENT_SHADER);
    GLuint vertShader = InitShader(vertFilePath, GL_VERTEX_SHADER);

    // Make sure Fragment Shaders Compiled Correctly
    // Attach & Link Shaders
    if (vertShader != -1 && fragShader != -1) {
        ID = glCreateProgram();

        glAttachShader(ID, vertShader);
        glAttachShader(ID, fragShader);
        glLinkProgram(ID);

        // Check for Linking Errors
        char infoLog[512];
        int success;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cerr << "Program Linking ERROR: Failed to link\n"
                        << infoLog;

            status = false;
        }

        // Success
        else {
            std::cout << "Program Shader Compiled Successfuly!\n";
            status = true;

            // Delete Shaders
            glDeleteShader(vertShader);
            glDeleteShader(fragShader);
        }
    }
}


GLuint InitShader(std::string srcFile, GLenum shaderType) {
    // Load in Source Code
    std::ifstream in(srcFile);
    if (!in.is_open())  //
        fprintf(stderr, "Shader Initialize: Source Code %s could not be loaded\n", srcFile.c_str());
    std::string vertSrc((std::istreambuf_iterator<char>(in)),  //
                        std::istreambuf_iterator<char>());
    in.close();

    const char *c_str = vertSrc.c_str();

    // Compile and Store Shader
    GLuint shaderID = glCreateShader(shaderType);  // Stores Reference ID
    glShaderSource(shaderID, 1, &c_str, NULL);
    glCompileShader(shaderID);


    // Check for Errors
    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cerr << "Initialize Shaders: Error in Compiling Shader Source!\n"
                  << infoLog;
        return -1;
    }

    // Return the Shader Reference ID Created
    return shaderID;
}

BufferData createBuffer(GLfloat *verticies, size_t vSize, GLuint *indicies, size_t iSize, GLuint programID) {
    /* 0. Allocate Verticies Buffer Object on GPU */
    GLuint VAO;                  // Vertex Array Object (Binds Vertex Buffer with the Attributes Specified)
    GLuint vBuffer;              // Vertex Buffer
    GLuint iBuffer;              // Element BUffer Object that specifies Order of drawing existing verticies
    glGenVertexArrays(1, &VAO);  // Create a VAO
    glGenBuffers(1, &vBuffer);   // Create One Buffer
    glGenBuffers(1, &iBuffer);   // Create Buffer for Indicies


    /* 0.5. Bind the VAO so that the data is stored in it */
    glBindVertexArray(VAO);


    /* 1. Specify how to Interpret the Vertex Data (Buffer Attribute) */
    // Bind Vertex Buffer Data
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);  // Tell OpenGL it's an Array Buffer

    /* Send the data into the Buffer Memory to Binded Buffer
	 *  GL_STATIC_DRAW:     the data will most likely not change at all or very rarely.
	 *  GL_DYNAMIC_DRAW:    the data is likely to change a lot.
	 *  GL_STREAM_DRAW:     the data will change every time it is drawn.
	 */
    glBufferData(GL_ARRAY_BUFFER, vSize, verticies, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);  // Enable aPos Attribute
    glVertexAttribPointer(         //
        0,                         // Which Index Attribute to Configure (At Location 0, aPos)
        3,                         // There are Values per Vertex (x,y,z)
        GL_FLOAT,                  // Type of Data in the Array
        GL_FALSE,                  // Normalize?
        7 * sizeof(float),         // Stride till next Vertex
        (void *)0                  // Pointer to the Beginning position in the Buffer
    );


    /* 2. Store Index Elements Data */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, indicies, GL_STATIC_DRAW);
    glDisableVertexAttribArray(0);  // Disable aPos Attribute


    /* 3. Configure RGB Attribute */
    GLuint aRGBA = glGetAttribLocation(programID, "aRGBA");
    glEnableVertexAttribArray(aRGBA);
    glVertexAttribPointer(aRGBA, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));

    /* 4. Object is ready to be Drawn */
    BufferData data(vBuffer, iBuffer, VAO);           // Create data Reference Object
    data.indiciesElts = iSize / sizeof(indicies[0]);  // Store Number of Indicies

    return data;
}



/* CHIP8 Section */
/**
 * Display Section
 */
Display::Display(CHIP8 *chip8) {
    // Configure CPU Settings
    cpu = chip8;

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Set OpenGL version and stencil size
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create Window
    window = SDL_CreateWindow("yac8 Interpreter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 256, SDL_WINDOW_OPENGL);

    // Create OpenGL Context for window
    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        std::cerr << "OpenGL Context could not be created! SDL Error: " << SDL_GetError() << '\n';
    } else {
        // Init GLEW
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();
        if (glewErr != GLEW_OK) {
            std::cerr << "Error Initializing GLEW! " << glewGetErrorString(glewErr) << '\n';
        }

        // Use VSync
        if (SDL_GL_SetSwapInterval(1) < 0) {
            std::cerr << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << '\n';
        }

        glEnable(GL_DEPTH_TEST);
    }
}

Display::~Display() {
    printf("\nExiting, cleaning up first...\n");

    /* Free Up Buffer Data */
    for (BufferData &bf : bufferData) {
        BufferData::freeBufferData(&bf);
    }

    // Destroy Context on exit
    SDL_GL_DeleteContext(context);
    SDL_Quit();
}

/**
 * Main Display Run Loop
 */
void Display::run() {
    // Create window event and start loop
    SDL_Event windowEvent;
    bool keyPos = true;     // False = KeyDown | True = KeyUp
    SDL_Scancode *keyCode;  // Stored Key ScanCode
    bool isCpuRun = false;  // DEBUG: Pause/Play

    // Preload Data
    Preload();

    // RUN LOOP
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            switch (windowEvent.type) {
            // Keyboard Event
            case SDL_KEYDOWN:
                // Key Pressed Down
                keyPos = false;

            case SDL_KEYUP:
                keyCode = &windowEvent.key.keysym.scancode;  // Store Key ScanCode

#if DISPLAY_KEY_DEBUG  // DEBUG: Output KeyPress
                std::cout << "KeyCode[ "
                          << (keyPos ? "UP" : "DOWN") << "] = "
                          << *keyCode << '\n';
#endif
#if DISPLAY_DEBUG_MODE  // DEBUG: Debug Mode Outputs
                // Debug Key
                if (!keyPos && *keyCode == SDL_SCANCODE_F1) {
                    std::cout << "=== DEBUG START ===\n";
                    cpu->regDump(std::cout);
                    cpu->keyDump(std::cout);
                    cpu->stackDump(std::cout);
                    std::cout << "=== DEBUG END ===\n\n";
                }
#endif

                // Set Key Value
                for (u_char i = 0x0; i <= 0xF; i++) {
                    if (*keyCode == keyMap[i])
                        cpu->key[i] = !keyPos;  // Set CPU's Key to Position Pressed
                }

                // DEBUG: Step by Step Run Instruction + Output to Console
                if (!keyPos && *keyCode == SDL_SCANCODE_SPACE) {
                    system("clear && date");
                    cpu->run(true);
                    cpu->displayDump(std::cout);
                    cpu->regDump(std::cout);
                    cpu->keyDump(std::cout);
                } else if (!keyPos && *keyCode == SDL_SCANCODE_RETURN) {
                    isCpuRun = !isCpuRun;
                }

                // Reset Key Pos
                keyPos = true;
                break;

            default:
                break;
            }

            // Check if close button was clicked
            if (windowEvent.type == SDL_QUIT) break;
        }

        // DEBUG: Play Run
        if (isCpuRun) {
            system("clear && date");
            cpu->run(true);
            cpu->displayDump(std::cout);
            cpu->regDump(std::cout);
            cpu->keyDump(std::cout);
        }

        // RENDER SECTION
        // Clear the Screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw Data
        Draw();

        // Swap front and back buffer
        SDL_GL_SwapWindow(window);
    }
}

void Display::Preload() {
    // Load in Default Shaders
    shader.compile("../Shaders/shader.vert", "../Shaders/shader.frag");

    // Create Object1
    // clang-format off
	GLfloat verticies[] = {
		// Positions<vec3>		RGBA<vec4>
		-0.4f, -0.2f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
		-0.2f, -0.2f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
		-0.4f,  0.2f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
		-0.2f,  0.2f, 0.0f,		1.0f, 1.0f, 1.0f, 1.0f,
	};

	GLuint indicies[] = {
		0, 1, 2,                // First Triangle
		1, 2, 3                 // Second Triangle
	};
    // clang-format on

    // Create and Bind Data to Buffer
    bufferData.push_back(
        createBuffer(verticies, sizeof(verticies), indicies, sizeof(indicies), shader.ID));


    // Create Object2
    // clang-format off
    GLfloat verticies2[] = {
		// Positions<vec3>		RGB<vec4>
        0.0f,  0.3f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,		// Bottom-Left
        0.4f,  0.3f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,		// Bottom-Right
		0.0f, -0.3f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,		// Top-Left
		0.4f, -0.3f, 0.0f,     0.5f, 0.0f, 0.5f, 1.0f,		// Top-Right
    };
    // clang-format on
    bufferData.push_back(
        createBuffer(verticies2, sizeof(verticies2), indicies, sizeof(indicies), shader.ID));


    // DEBUG: Output Data Created
    int i = 0;
    for (BufferData &bd : bufferData) {
        std::cout << "Buffer[" << i << "]:\n";
        std::cout << "\tIndexBuffer: " << bd.indiciesBuffer << '\n';
        std::cout << "\tIndexElements: " << bd.indiciesElts << '\n';
        std::cout << "\tVertexBuffer: " << bd.verticiesBuffer << '\n';
    }
}

void Display::Draw() {
    // Draw the Object
    shader.use();

    // Render all Buffer Data
    for (BufferData &bd : bufferData) {
        // Enable aPos Attribute
        glEnableVertexAttribArray(0);

        // Bind Vertex Array Object
        glBindVertexArray(bd.VAO);

        // Bind Indicies
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bd.indiciesBuffer);

        // Draw the Elements
        glDrawElements(GL_TRIANGLES, bd.indiciesElts, GL_UNSIGNED_INT, nullptr);

        // Finished with aPos Attribute
        glDisableVertexAttribArray(0);
    }
}