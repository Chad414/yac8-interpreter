#pragma once

// Core Libraries
#include <cstring>
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <stdio.h>

// OpenGL Libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Project Libraries
#include "BufferData.h"



class SimpleRender {
  protected:  // Protected Variables | GL Window Data
    const unsigned int WIDTH = 400;
    const unsigned int HEIGHT = 400;
    double FPS;  // Current Calculated FPS Value
    uint32_t overallFrameCount = 0;

  protected:  // Shared Window Data
    GLFWwindow* window;
    const char* title = "GLFW Window";
    char titleBuffer[256];  // Used for Temporary Character Storage (Window Title)



  protected:  // Structure for Better Shader Handling
    static struct Shader {
        bool status;  // Keep track of Shader Status (False = Not Ready | True = Ready)
        GLuint ID;    // Store Compiled Shader Program

        Shader() : ID(0), status(false){};             // No Shader Given
        Shader(GLuint _id) : ID(_id), status(true){};  // Initialize Shader to precompiled Program

        void use();                              // Uses Current Program (If any)
        void compile(const char*, const char*);  // Compiles Given Shader Files (Vertex, Fragment)
    } s;                                         // TODO: Fix Later

  protected:  // Shared Variables
    Shader defaultShader;
    std::vector<BufferData> bufferData;  // Store References the Buffer Data



  private:  // Private Methods (Static - Callbacks)
    /* Called when Key Pressed */
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /* Mouse Click Callback */
    static void mouseBtn_callback(GLFWwindow* window, int button, int action, int mods);

    /* Mouse Cursor Position Relative to Window Callback */
    static void cursorPos_callback(GLFWwindow* window, double xPos, double yPos);

    /* Mouse Cursor Scroll Offset Callback */
    static void mouseScroll_callback(GLFWwindow* window, double xOffset, double yOffset);

    /* GLFW Error Callback */
    static void error_callback(int error, const char* description);


  protected:  // Shared Overrideable Callbacks
    virtual void onKey(int key, int scancode, int action, int mods);
    virtual void onMouseClick(int button, int action, int mods);
    virtual void onMouseScroll(double xOffset, double yOffset);
    virtual void onMouse(double xPos, double yPos);


  protected:  // Shared Methods
    /**
     * Initializes Source Code of given shaderType
     * @param srcFile - The Source Code path for the Shader
     * @param shaderType - The Shader Type
     * @return - Shader Reference ID, Returns -1 if Failed
     */
    static GLuint InitShader(std::string srcFile, GLenum shaderType);

    /**
     * Creates Buffer data for Verticies & Indicies provided
     *  by creating a VAO linked to a VBO and EBO.
     * Data is configured and packaged in an Object with the
     *  reference IDs given by OpenGL and returned.
     *
     * @param verticies - The Verticies Array, seperated by (x,y,z)/Vertex
     * @param vSize     - Size of the array in Bytes (sizeof(verticies))
     * @param indicies - The Indicies Array, specifying the order of Vertex to be drawn
	 * @param iSize     - Size of the array in Bytes (sizeof(indicies))
	 * @param programID - Program ID of Compiled Shaders
     * @return BufferData Object with the Object Reference IDs stored
     */
    static BufferData createBuffer(GLfloat* verticies, size_t vSize, GLuint* indicies, size_t iSize, GLuint programID);

    /**
	 * Returns the Calculated Frames Per Second from Draw Loop
	 *	@returns FPS Value
	 */
    const double getFPS();



  private:  // Helper Functions
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
     * Constuctor that initalizes GLFW with given Width and Height
     * @param w - The Width of the Window
     * @param h - The Height of the Window
     */
    SimpleRender(unsigned int w, unsigned int h, const char* title = "GLFW Window");

    /**
     * Destructor to clean up Resources used by OpenGL
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
};
