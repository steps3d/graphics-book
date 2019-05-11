//
// Simple OpenGL utility functions
//

#pragma once
#ifndef	__GL_UTILITIES__
#define	__GL_UTILITIES__

#include	<string.h>

		// get current OpenGL error desc or NULL if no error
const char * getGlErrorString ();

		// get bound framebuffer error or NULL if no error
const char * checkFramebuffer ();

		// check for error and print if any
void checkGlError ( const char * title );

		// make a screenshot
bool	saveScreenAsTga ( const char * fileName, int w, int h );

#endif
