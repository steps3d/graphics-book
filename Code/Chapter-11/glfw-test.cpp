//
// Simple test of GLFW library

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <stdio.h>
#include "TgaImage.h"

void makeScreenshot ( GLFWwindow * window )
{
	int width, height;
	
	glfwGetFramebufferSize ( window, &width, &height );

	uint8_t * ptr    = (uint8_t *) malloc ( 3 * width * height );

	if ( ptr == nullptr )
		return;

	TgaImage image ( width, height );
	
                                            // get image from window
	glPixelStorei ( GL_PACK_ALIGNMENT, 1 );                         // set 1-byte alignment
	glFinish      ();
	glReadBuffer  ( GL_FRONT );
	glReadPixels  ( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, ptr );

	image.setRgbData ( ptr );				// it should do RGB->BGR mapping
	
	free ( ptr );
                                            // now save it as 24-bit uncompressed tga
	image.writeToFile ( "glfw-test.tga" );
}

void error ( int error, const char * description )
{
	fputs ( description, stderr );
}

void key ( GLFWwindow * window, int key, int scancode, int action, int mods )
{
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose ( window, GL_TRUE );

	if ( key == GLFW_KEY_F1 && action == GLFW_RELEASE )
		makeScreenshot ( window );
}

void display ( GLFWwindow * window )
{
	int width, height;
	
	glfwGetFramebufferSize ( window, &width, &height );

	float	ratio = width / (float) height;
	
	glViewport(0, 0, width, height);
	glClearColor ( 0.5, 0.5, 0.5, 1.0 );
	glClear(GL_COLOR_BUFFER_BIT);
}

int main ()
{
	glfwSetErrorCallback ( error );
	
	if ( !glfwInit() )
		exit ( 1 );
	
	GLFWwindow * window = glfwCreateWindow ( 800, 800, "Simple example", NULL, NULL );
	
	if ( !window )
	{
		glfwTerminate ();
		exit ( 1 );
	}
	
	glfwMakeContextCurrent ( window );
	glfwSwapInterval       ( 1 );
	glfwSetKeyCallback     ( window, key );
	
	while ( !glfwWindowShouldClose ( window ) )
	{
		display ( window );
		glfwSwapBuffers ( window );
		glfwPollEvents  ();
	}
	
	glfwDestroyWindow ( window );
	glfwTerminate     ();
	
	return 0;
}
