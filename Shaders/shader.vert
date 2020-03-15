#version 460
/*
 * Vertex Shader is responsible for the Location
 *  of objects
 */


/* Incomming Data */
layout (location = 0) in vec3 aPos;			// Postion of Variable from Location 0
in vec4 aRGBA;								// RGBA Color of Vertex

/* Outbound Data */
out vec4 vertexColor;						// Vector of Color outputing to Fragment Shader


void main() {
	// Outbound Data
	vertexColor = aRGBA;
	gl_Position = aPos;
}