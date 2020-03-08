#version 460 core
/*
 * Fragment Shader is responsible for the color
 *  output for each pixel
 */

/* Shader Settings */
precision mediump float;    // Set Percition to Medium

/* Incomming Data */
in vec4 vertexColor;		// Color from Vertex -> Given to Fragment to Apply

/* Outbound Data */
out vec4 FragColor;			// Color of Object -> Apply


/* Uniform Data */

void main() {
	// Use RGBA Color
	FragColor = vertexColor;
}