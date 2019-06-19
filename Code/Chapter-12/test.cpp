//
// Example of using freeglut and glew to get OpenGL 3.3/4.1
//
// Author: Alex V. Boreskoff <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutWindow.h"

#include	<stdio.h>
#include	<cstdlib>
#include	<cstring>

#define	NUM_VERTICES	3
#define	VERTEX_SIZE		(3*sizeof(float))

static const float vertices [] = 
{
	-1.0f, -1.0f, 0.0f,
	 0.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f
};

class	TestWindow : public GlutWindow
{
	GLuint	program;
	GLuint  vertexShader, fragmentShader;
	GLuint	vao;
	GLuint	vbo;
	int		status;
	
public:
	TestWindow () : GlutWindow  ( 200, 100, 900, 900, "Test" )
	{
		createProgram ();
		
		glUseProgram ( program );

		createBuffers ();

		setAttrPtr ( "pos", 3, VERTEX_SIZE, nullptr );
	}

	void	createBuffers ()
	{
		glGenVertexArrays ( 1, &vao );
		glBindVertexArray ( vao );
		glGenBuffers      ( 1, &vbo );
		glBindBuffer      ( GL_ARRAY_BUFFER, vbo );
		glBufferData      ( GL_ARRAY_BUFFER, NUM_VERTICES * VERTEX_SIZE, vertices, GL_STATIC_DRAW );
	}
	
	void	createProgram ()
	{
		vertexShader   = loadShaderFromFile ( "simple.vsh", GL_VERTEX_SHADER );
		fragmentShader = loadShaderFromFile ( "simple.fsh", GL_FRAGMENT_SHADER );
				
		if ( !vertexShader || !fragmentShader )
			::exit ( 1 );
			
		program = glCreateProgram ();

		glAttachShader ( program, vertexShader   );
		glAttachShader ( program, fragmentShader );
		glLinkProgram  ( program );		
		glGetProgramiv ( program, GL_LINK_STATUS, &status );
	  
		if ( status != GL_TRUE )
		{
			int length;
		
			glGetProgramiv ( program, GL_INFO_LOG_LENGTH, &length );
		
			char * buf = (char *) malloc ( length );
		
			glGetProgramInfoLog ( program, length, nullptr, buf );
		
			printf ( "Link failure.\n%s\n", buf );
			free   ( buf );
		
			::exit ( 1 );
		}
	}
	
	GLuint   loadShaderFromFile ( const char * fileName, GLenum type )
	{
		printf ( "Loading %s\n", fileName );

		FILE * file = fopen ( fileName, "rb" );
		
		if ( file == nullptr )
		{
			printf ( "Error opening %s\n", fileName );
			
			return 0;
		}

		fseek ( file, 0, SEEK_END );
		
		GLint	size = ftell ( file );
		
		if ( size < 1 )
		{
			fclose ( file );
			printf ( "Error loading file %s\n", fileName );
			
			return 0;
		}
		
		char * buf = (char *) malloc ( size + 1 );
		
		fseek ( file, 0, SEEK_SET );
		
		if ( fread ( buf, 1, size, file ) != size )
		{
			fclose ( file );
			free   ( buf );
			printf ( "Error loading file %s\n", fileName );
			
			return 0;
		}

		fclose ( file );
		
		buf [size] = '\0';
		
		GLuint shader = loadShader ( buf, type );
		
		free ( buf );
		
		return shader;
	}
	
	GLuint loadShader ( const char * source, GLenum type )
	{
		GLuint shader = glCreateShader ( type );
		GLint status, length;

		if ( shader == 0 )
			return 0;
						// pass all source as one string
		glShaderSource  ( shader, 1, &source, nullptr );
		glCompileShader ( shader );
		glGetShaderiv   ( shader, GL_COMPILE_STATUS, &status );

		if ( status != GL_TRUE )
		{
			glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &length );

			char * buf = (char *) malloc ( length );

			glGetShaderInfoLog ( shader, length, nullptr, buf );

			printf ( "Compile failure.\n%s\n", buf );
			free   ( buf );

			glDeleteShader ( shader );

			return 0;
		}

		return shader;
	}
	
	void	setAttrPtr ( const char * name, int numComponents, GLsizei stride, void * ptr, 
	                     GLenum type = GL_FLOAT, bool normalized = false )
	{
		int	loc = glGetAttribLocation ( program, name );

		if ( loc < 0 )
			::exit ( 1 );
			
		glVertexAttribPointer ( loc, numComponents, 
								type, normalized ? GL_TRUE : GL_FALSE,
								stride, (const GLvoid*) ptr );			
		glEnableVertexAttribArray ( loc );
	}

	void	redisplay () override
	{
		glClear           ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glBindVertexArray ( vao );
		glDrawArrays      ( GL_TRIANGLES, 0, NUM_VERTICES );
		glBindVertexArray ( 0 );
	}
	
	void	keyTyped ( unsigned char key, int modifiers, int x, int y ) override
	{
		if ( key == 27 || key == 'q' || key == 'Q' )	//	quit requested
			exit ( 0 );
	}
};

int main ( int argc, char * argv [] )
{
								// initialize glut
	GlutWindow::init ( argc, argv );
	
	TestWindow	win;
	
	return GlutWindow::run ();
}
