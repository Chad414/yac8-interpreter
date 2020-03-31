//
// Created by chad on 3/6/20.
//

#include "../include/Display.h"

/* Helper Functions */
// Map value x that is between a-b to c-d
float map(float x, float a, float b, float c, float d) {
    return (x - a) / (b - a) * (d - c) + c;
}

/* SimpleRender Section */
void Display::Preload() {
    // Load in Default Shaders
    defaultShader.compile("./Shaders/shader.vert", "./Shaders/shader.frag");

	// Allocate Buffer Data to Screen Size
    bufferData.resize(64 * 32);
    std::cout << "Buffer Size: " << bufferData.size() << '\n';

    // Setting Up Verticies
    {
        /*
        TODO: Check if display[x][y] is set, if so
            use the map() function to map location
            to -1, 1 and then create a buffer to set that.
        Need to also make a Square at that location
            FROM CENTER!

        NOTE: Later Optimization could be to create an entire
            raw array of only verticies (remove RGB) and manipulate
            those values based on location and send to GPU
        */
        // DEBUG: Some Point
        float x = 10.f;
        float y = 10.f;


        // Ratio of Window Resolution to CHIP8 Actual Res
        const float ratio_X = WIDTH / 64;
        const float ratio_Y = HEIGHT / 32;

        std::cout << "Ratio_X: " << ratio_X << '\n';
        std::cout << "Ratio_Y: " << ratio_Y << '\n';
        std::cout << '\n';

        /* DEBUG: Relative View Resolution Mapping */
        std::cout << "Relative View:\n";
        std::cout << "\tx: " << x << '\n';
        std::cout << "\ty: " << y << '\n';

        // Generate Points for Box
        float r_x1 = x - (ratio_X / 4);
        float r_y1 = y + (ratio_Y / 4);
        float r_x2 = x + (ratio_X / 4);
        float r_y2 = y + (ratio_Y / 4);
        float r_x3 = x + (ratio_X / 4);
        float r_y3 = y - (ratio_Y / 4);
        float r_x4 = x - (ratio_X / 4);
        float r_y4 = y - (ratio_Y / 4);

        std::cout << "\n\tx1: " << r_x1 << '\n';
        std::cout << "\ty1: " << r_y1 << '\n';
        std::cout << "\tx2: " << r_x2 << '\n';
        std::cout << "\ty2: " << r_y2 << '\n';
        std::cout << "\tx3: " << r_x3 << '\n';
        std::cout << "\ty3: " << r_y3 << '\n';
        std::cout << "\tx4: " << r_x4 << '\n';
        std::cout << "\ty4: " << r_y4 << '\n';



        /* OpenGL View Resolution Mapping */
        std::cout << "\n\nMapped View:\n";

        // Map to OpenGL Res
        float vx = map(x, 0, 64, -1, 1);
        float vy = map(y, 0, 32, -1, 1);

        std::cout << "\tMappedLoc_X: " << vx << '\n';
        std::cout << "\tMappedLoc_Y: " << vy << '\n';
        std::cout << '\n';

        // Generate Points for Box
        float x1 = map(r_x1, 0, 64, -1, 1);
        float y1 = map(r_y1, 0, 32, -1, 1);
        float x2 = map(r_x2, 0, 64, -1, 1);
        float y2 = map(r_y2, 0, 32, -1, 1);
        float x3 = map(r_x3, 0, 64, -1, 1);
        float y3 = map(r_y3, 0, 32, -1, 1);
        float x4 = map(r_x4, 0, 64, -1, 1);
        float y4 = map(r_y4, 0, 32, -1, 1);

        std::cout << "\tx1: " << x1 << '\n';
        std::cout << "\ty1: " << y1 << '\n';
        std::cout << "\tx2: " << x2 << '\n';
        std::cout << "\ty2: " << y2 << '\n';
        std::cout << "\tx3: " << x3 << '\n';
        std::cout << "\ty3: " << y3 << '\n';
        std::cout << "\tx4: " << x4 << '\n';
        std::cout << "\ty4: " << y4 << '\n';



        // DEBUG: Test the box :)
        // clang-format off
        GLfloat verticies[] = {
            // VERTEX<vec3>		RGBA<vec4>
            x1, y1, 0.f,        1.0f, 1.0f, 1.0f, 1.0f,  // [0,0]        0
            x2, y2, 0.f,        1.0f, 1.0f, 1.0f, 1.0f,  // [1,1]        1
            x3, y3, 0.f,        1.0f, 1.0f, 1.0f, 1.0f,  // [1,0]        2
            x4, y4, 0.f,        1.0f, 1.0f, 1.0f, 1.0f   // [0,1]        3
        };

        GLuint indicies[] = {
            3, 0, 1,
            3, 2, 1};
        // clang-format on


		bufferData[0] = createBuffer(verticies, sizeof(verticies), indicies, sizeof(indicies), defaultShader.ID);
    }



    // Display some Internal Info
    int nrAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttribs);
    std::cout << "Maximum number of Vertex Attributes Supported: " << nrAttribs << std::endl;
}


void Display::Draw() {
    if (defaultShader.status) {
        defaultShader.use();  // Use Default Program
    }

    // Output FPS to Window Title
    sprintf(titleBuffer, "%s [%.2f FPS]", title, getFPS());
    glfwSetWindowTitle(window, titleBuffer);


    // Draw From the Buffer
    for (BufferData &bd : bufferData) {
        // Enable aPos Attribute
        glEnableVertexAttribArray(0);

        // Bind Vertex Array Object
        glBindVertexArray(bd.VAO);

        // Bind Index Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bd.indiciesBuffer);

        // Draw
        glDrawElements(GL_TRIANGLES, bd.indiciesElts, GL_UNSIGNED_INT, nullptr);

        // Disable aPos Attribute
        glDisableVertexAttribArray(0);
    }

    // Generate new Buffer Data from CHIP8 Display
    if(!((overallFrameCount+1) % 5)) {
        // Clear Entire Buffer if Triggered to do So
        if (cpu->clsTrigger) {
			for(BufferData &bf : bufferData) {
				BufferData::freeBufferData(&bf);
			}

			cpu->clsTrigger = false;	// Reset Trigger
        }


		// Calculate Offsets
        float x_off = 0.012f * ((64/float(WIDTH))*10);
        float y_off = 0.04f * ((32/float(HEIGHT))*10);


		// Check Changes to Display
        while (!cpu->deltaDisplay.empty()) {
			// Get Pixel to Update
            Pixel &p = cpu->deltaDisplay.top();

			// Clear Buffer at Pixel
			//  This way if Pixel was removed, it's removed prior
            BufferData::freeBufferData(&bufferData[(p.y * 64) + p.x]);

			// Check if Pixel Turned on
            if (p.val) {
				// Map to OpenGL Res
				float vx = map(float(p.x), 0, 64, -1, 1);
				float vy = map(float(p.y), 0, 32, -1, 1);

				// Generate Points for Box
				float x1 = vx - x_off;
				float y1 = -vy + y_off;
				float x2 = vx + x_off;
				float y2 = -vy + y_off;
				float x3 = vx + x_off;
				float y3 = -vy - y_off;
				float x4 = vx - x_off;
				float y4 = -vy - y_off;


				// Create Data for Buffer
				// clang-format off
				GLfloat verticies[] = {
					// VERTEX<vec3>		RGBA<vec4>
					x1, y1, 0.f,        1.0f, 1.0f, 1.0f, 1.0f,  // [0,0]        0
					x2, y2, 0.f,        1.0f, 1.0f, 1.0f, 1.0f,  // [1,1]        1
					x3, y3, 0.f,        1.0f, 1.0f, 1.0f, 1.0f,  // [1,0]        2
					x4, y4, 0.f,        1.0f, 1.0f, 1.0f, 1.0f   // [0,1]        3
				};

				GLuint indicies[] = {
					3, 0, 1,
					3, 2, 1};
				// clang-format on


				// Update Buffer Associated with Pixel
				bufferData[(p.y * 64) + p.x] = createBuffer(verticies, sizeof(verticies), indicies, sizeof(indicies), defaultShader.ID);
            }


			// Pop off Updated Pixel
            cpu->deltaDisplay.pop();
		}


        // Run CHIP8
        cpu->run(true);
         //cpu->displayDump(std::cout);
        // cpu->regDump(std::cout);
        // cpu->keyDump(std::cout);
    }

}

/**
 * Handle Key Press on CHIP8 Key Map
 */
void Display::onKey(int key, int scancode, int action, int mods) {
    // Adjust Transformation
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        // Set Key Value
        for (u_char i = 0x0; i <= 0xF; i++) {
            if (key == keyMap[i])
                cpu->key[i] = (action == GLFW_PRESS);  // Set CPU's Key to Position Pressed
        }

        // Exit Key?
        // if (key == GLFW_KEY_Q)
        //     glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


/* CHIP8 Section */
/**
 * Display Section
 */
Display::Display(CHIP8 *chip8) : SimpleRender(512, 256, "YAC8") {
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