#include	<GL/freeglut.h>
#include	"TgaImage.h"

void makeScreenshot ()
{
	int	  width  = glutGet ( GLUT_WINDOW_WIDTH  );
	int	  height = glutGet ( GLUT_WINDOW_HEIGHT );
	uint8_t * ptr    = (uint8_t *) malloc ( 3 * width * height );

	if ( ptr == nullptr )
        	return;

	TgaImage image ( width, height );
	
	glPixelStorei ( GL_PACK_ALIGNMENT, 1 );                         // set 1-byte alignment
	glFinish      ();
	glReadBuffer  ( GL_FRONT );
	glReadPixels  ( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, ptr );

	image.setRgbData ( ptr );				// it should do RGB->BGR mapping
	
	free ( ptr );
                                            // now save it as 24-bit uncompressed tga
	image.writeToFile ( "test-simple.tga" );
}

void display ()
{
	glClearColor ( 0.5, 0.5, 0.5, 1.0 );
	glClear      ( GL_COLOR_BUFFER_BIT );

	glutSwapBuffers ();
}

void reshape ( int w, int h )
{
	glViewport ( 0, 0, w, h );
}

void key ( unsigned char key, int x, int y )
{
	if ( key == 27 || key == 'q' || key == 'Q' )        // quit requested
		exit ( 0 );

	if ( key == 's' )
		makeScreenshot ();
}

int main ( int argc, char * argv [] )
{
	glutInit ( &argc, argv );

	glutInitDisplayMode    ( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowPosition ( 100, 100 );
	glutInitWindowSize     ( 800, 600 );
	glutCreateWindow       ( "Example" );

	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );

	glutMainLoop ();

	return 0;
}

