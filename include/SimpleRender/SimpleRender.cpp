#include "SimpleRender.h"

/**
 ***********************************************************
 * Private Static Methods and Callbacks
 *  - Key Presses, Mouse Button, Cursor Movements, and
 *      Mouse Scrolling Callbacks
 ***********************************************************
 */

void SimpleRender::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	// Obtain and Cast Object Reference
	void* r = glfwGetWindowUserPointer(window);
	if (r != NULL) {
		SimpleRender* obj = static_cast<SimpleRender*>(r);
		obj->onKey(key, scancode, action, mods);			// Call Overrideable Function
	}
}
void SimpleRender::onKey(int key, int scancode, int action, int mods) {
	// Output Key Pressed
	printf("KEY: Key[%d], ScanCode[%d], Action[%d], Mods[%d]\n", key, scancode, action, mods);

	// Close window on 'Q' Press
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void SimpleRender::mouseBtn_callback(GLFWwindow *window, int button, int action, int mods) {
	// Obtain and Cast Object Reference
	void* r = glfwGetWindowUserPointer(window);
	if (r != NULL) {
		SimpleRender* obj = static_cast<SimpleRender*>(r);
		obj->onMouseClick(button, action, mods);			// Call Overrideable Function
	}
}
void SimpleRender::onMouseClick(int button, int action, int mods) {
	// Output Key Pressed
	printf("MOUSE: Button[%d], Action[%d], Mods[%d]\n", button, action, mods);
}

void SimpleRender::cursorPos_callback(GLFWwindow *window, double xPos, double yPos) {
	// Obtain and Cast Object Reference
	void* r = glfwGetWindowUserPointer(window);
	if (r != NULL) {
		SimpleRender* obj = static_cast<SimpleRender*>(r);
		obj->onMouse(xPos, yPos);			// Call Overrideable Function
	}
}
void SimpleRender::onMouse(double xPos, double yPos) {
	// Output Mouse Cursor Position
	printf("CURSOR: X[%.2f], Y[%.2f]\n", xPos, yPos);
}

void SimpleRender::mouseScroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
	// Obtain and Cast Object Reference
	void* r = glfwGetWindowUserPointer(window);
	if (r != NULL) {
		SimpleRender* obj = static_cast<SimpleRender*>(r);
		obj->onMouseScroll(xOffset, yOffset);			// Call Overrideable Function
	}
}
void SimpleRender::onMouseScroll(double xOffset, double yOffset) {
	// Output Mouse Cursor Position
	printf("SCROLL: X-off[%.2f], Y-off[%.2f]\n", xOffset, yOffset);
}

void SimpleRender::error_callback(int error, const char *description) {
    // Output any Errors
    fprintf(stderr, "Error[%d]: %s\n", error, description);
}


/**
 ***********************************************************
 * Private Static Methods Backend
 * Initialization of Backend Functionallity
 *		-Shaders
 *		-Buffers
 ***********************************************************
 */

GLuint SimpleRender::InitShader(std::string srcFile, GLenum shaderType) {
    // Load in Source Code
    std::ifstream in(srcFile);
    if (!in.is_open()) //
        fprintf(stderr, "Shader Initialize: Source Code %s could not be loaded\n", srcFile.c_str());
    std::string vertSrc((std::istreambuf_iterator<char>(in)), //
                        std::istreambuf_iterator<char>());
    in.close();

    const char *c_str = vertSrc.c_str();

    // Compile and Store Shader
    GLuint shaderID = glCreateShader(shaderType); // Stores Reference ID
    glShaderSource(shaderID, 1, &c_str, NULL);
    glCompileShader(shaderID);


    // Check for Errors
    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cerr << "Initialize Shaders: Error in Compiling Shader Source!\n" << infoLog;
        return -1;
    }

    // Return the Shader Reference ID Created
    return shaderID;
}

BufferData SimpleRender::createBuffer(GLfloat *verticies, size_t vSize, GLuint *indicies, size_t iSize, GLuint programID) {
	/* 0. Allocate Verticies Buffer Object on GPU */
	GLuint VAO;					// Vertex Array Object (Binds Vertex Buffer with the Attributes Specified)
	GLuint vBuffer;             // Vertex Buffer
    GLuint iBuffer;             // Element BUffer Object that specifies Order of drawing existing verticies
	glGenVertexArrays(1, &VAO);	// Create a VAO
    glGenBuffers(1, &vBuffer);  // Create One Buffer
    glGenBuffers(1, &iBuffer);  // Create Buffer for Indicies



	/* 0.5. Bind the VAO so that the data is stored in it */
	glBindVertexArray(VAO);


    /* 1. Specify how to Interpret the Vertex Data (Buffer Attribute) */
	// Bind Vertex Buffer Data
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer); // Tell OpenGL it's an Array Buffer

	/* Send the data into the Buffer Memory to Binded Buffer
	 *  GL_STATIC_DRAW:     the data will most likely not change at all or very rarely.
	 *  GL_DYNAMIC_DRAW:    the data is likely to change a lot.
	 *  GL_STREAM_DRAW:     the data will change every time it is drawn.
	 */
	glBufferData(GL_ARRAY_BUFFER, vSize, verticies, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);  // Enable aPos Attribute
    glVertexAttribPointer( //
        0,                 // Which Index Attribute to Configure (At Location 0, aPos)
        3,                 // There are Values per Vertex (x,y,z)
        GL_FLOAT,          // Type of Data in the Array
        GL_FALSE,          // Normalize?
        7 * sizeof(float), // Stride till next Vertex
        (void *)0          // Pointer to the Beginning position in the Buffer
    );


	/* 2. Store Index Elements Data */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, indicies, GL_STATIC_DRAW);
	glDisableVertexAttribArray(0);			// Disable aPos Attribute


	/* 3. Configure RGB Attribute */
	GLuint aRGBA = glGetAttribLocation(programID, "aRGBA");
	glEnableVertexAttribArray(aRGBA);
	glVertexAttribPointer( aRGBA, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)) );


    /* 4. Object is ready to be Drawn */
    BufferData data(vBuffer, iBuffer, VAO);               // Create data Reference Object
    data.indiciesElts = iSize / sizeof(indicies[0]); // Store Number of Indicies

    return data;
}

const double SimpleRender::getFPS() {
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
    // Draw the Object
	defaultShader.use();

	// Output FPS to Window Title
	sprintf(titleBuffer, "%s [%.2f FPS]", title, getFPS());
	glfwSetWindowTitle(window, titleBuffer);


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

void SimpleRender::Preload() {
	// Load in Default Shaders
	defaultShader.compile("Shaders/shader.vert", "Shaders/shader.frag");

	// Create Object1
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

	// Create and Bind Data to Buffer
	bufferData.push_back(                                                      //
		createBuffer(verticies, sizeof(verticies), indicies, sizeof(indicies), defaultShader.ID) //
	);


	// Create Object2
    GLfloat verticies2[] = {
		// Positions<vec3>		RGB<vec4>
        0.0f,  0.3f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,		// Bottom-Left
        0.4f,  0.3f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,		// Bottom-Right
		0.0f, -0.3f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,		// Top-Left
		0.4f, -0.3f, 0.0f,     0.5f, 0.0f, 0.5f, 1.0f,		// Top-Right
    };
    bufferData.push_back(                                                        //
        createBuffer(verticies2, sizeof(verticies2), indicies, sizeof(indicies), defaultShader.ID) //
    );


	// DEBUG: Output Data Created
	int i = 0;
	for (BufferData& bd : bufferData) {
		std::cout << "Buffer[" << i << "]:\n";
		std::cout << "\tIndexBuffer: " << bd.indiciesBuffer << '\n';
		std::cout << "\tIndexElements: " << bd.indiciesElts << '\n';
		std::cout << "\tVertexBuffer: " << bd.verticiesBuffer << '\n';
	}
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

SimpleRender::SimpleRender(unsigned int  w, unsigned int  h, const char *title) : WIDTH(w), HEIGHT(h) {
    this->title = title;
    InitRender();
}

SimpleRender::~SimpleRender() {
	printf("\nExiting, cleaning up first...\n");


    /* Free Up Buffer Data */
    for(BufferData &bf : bufferData) {
        BufferData::freeBufferData(&bf);
    }
    
    /* Destroy Resources */
    glfwDestroyWindow(window);
    glfwTerminate();
}

void SimpleRender::InitRender() {
    /* Configure OpenGL Properties */
    glfwWindowHint(GLFW_SAMPLES, 4);                               // 4x Antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Enables Forward Compatiblity
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL Profile used
}

int SimpleRender::run() {
    /* Initialize GLFW */
    glewExperimental = true; // Needed for Core Profile
    if (!glfwInit()) {
        fprintf(stderr, "Failed to Initialize GLFW\n");
        glfwTerminate();
        return -1;
    }


    /* Open a Window with OpenGL Context */
    window = glfwCreateWindow(WIDTH, HEIGHT, title, NULL, NULL);
	glViewport(0, 0, WIDTH, HEIGHT);		// Set Rendering Dimensions

    if (!window) {
        fprintf(stderr, "Failed to open GLFW window!");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW


    /* Setup GLFW Properties */
    glfwSwapInterval(1); // Default is 0, this is to prevent Tearing
	glEnable(GL_DEPTH_TEST);


    /* Initialize GLEW */
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to Initalize GLEW\n");
        glfwTerminate();
        return -1;
    }

	/* Setup Input Callbacks */
	glfwSetWindowUserPointer(window, this);			// Keep track of Current Object
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouseBtn_callback);
	glfwSetCursorPosCallback(window, cursorPos_callback);
	glfwSetScrollCallback(window, mouseScroll_callback);
	glfwSetErrorCallback(error_callback);


	/* Keep track of FPS & Fixed Upate */
	double lastTime = glfwGetTime();
	int frameCount = 0;

    /* Run Pre-Start Function */
    Preload();

	

    /* Keep Window open until 'Q' key is pressed */
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    do {
		// Measure the Speed (FPS)
		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - lastTime >= 1.0) {
			FPS = frameCount;
			frameCount = 0;
			lastTime += 1.0;
		}


        // Clear the Screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw here...
        Draw();


        // Swap Buffers
        glfwSwapBuffers(window);

        // Wait for Polling Events
        glfwPollEvents();

		// Keep Track of Overall FrameCount
        overallFrameCount++;
    } while (!glfwWindowShouldClose(window));  // Keep Window Open util Window should Closed

    // No Issues
    return 0;
}


/* 
 ***********************************************************
 * Shader Structure
 *
 *	- Ease of Use for Shaders
 *  - Compiles and Stores Shader Attributes
 ***********************************************************
 */

void SimpleRender::Shader::use() {
	if (status)
		glUseProgram(ID);	// Make Sure ther IS a Valid Program ID
	else
		std::cerr << "Shader Struct: No Program to use!\n";
}

void SimpleRender::Shader::compile(const char* vertFilePath, const char* fragFilePath) {
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
			std::cerr << "Program Linking ERROR: Failed to link\n" << infoLog;

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