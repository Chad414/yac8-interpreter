#pragma once

// Core libraries
#include <GL/glew.h>

/**
 * Stores Data Objects of the There
 *  linked buffers.
 *      - Vertex Array Object       (VAO)
 *          - Binds the Vertex Attributes and Vertex Buffer
 *      - Vertex Buffer
 *      - Index Buffer
 *      - Number of Elements Indicies
 *		- Texture Object
 *
 */
class BufferData {
  private:

  public:	// Public Variables
	  GLuint	VAO;					// Vertex Array Object
	  GLuint	verticiesBuffer;		// Vertex Buffer
	  GLuint	indiciesBuffer;			// Index Buffer
	  size_t	indiciesElts = 0;		// Number of Indicies

  public:
	/* Default Constructor: Initialize everything to 0 */
	BufferData();

	/* 
	 * Construct Data based on Given Index Values
	 *	@param _vertBuffer - Vertex Buffer that holds all Verticies
	 *	@param _indBuffer - Index Buffer for the Verticies
	 *	@param _vao - Vertex Array Object that is bound to the Attributes
	 *		as well as the Vertex Buffer
	 */
	BufferData(GLuint&, GLuint&, GLuint&);

	/* Method that frees up used Memory */
	static void freeBufferData(BufferData*);	
};