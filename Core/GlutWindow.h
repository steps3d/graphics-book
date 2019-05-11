//
// Simple wrapper class for freeglut
//

#ifndef	__GLUT_WINDOW__
#define	__GLUT_WINDOW__

#include	<GL/glew.h>

#ifdef	_WIN32
	#include	<GL/wglew.h>
#else
	#include	<GL/glxew.h>
#endif

#include	<string>
#include	<GL/freeglut.h>
#include	"common.h"

#ifndef	_WIN32
	#define	CALLBACK
#endif

class	GlutWindow
{
	std::string	caption;
	int			width, height;
	int			id;
	float		time;				// time since last redisplay ()
	bool		debugEnabled;		// whether debug output callback activated
	std::string	screenshotName;
	int			screenshotKey;
	int			screenshotCount;
	float		frameTime [5];		// time at last 5 frames
	float		fps;
	int			frame   = 0;
	bool		showFps = true;
public:
	GlutWindow ( int xo, int yo, int w, int h, const char * caption );
	
	GlutWindow ( const GlutWindow& ) = delete;
	GlutWindow& operator = ( const GlutWindow& ) = delete;

	virtual ~GlutWindow ();
	
	int	getWidth () const
	{
		return width;
	}
	
	int	getHeight () const
	{
		return height;
	}
	
	float	getAspect () const
	{
		return static_cast<float> ( width ) / static_cast<float> ( height );
	}

	const std::string& getCaption () const
	{
		return caption;
	}

	void	setCaption ( const std::string& newCaption )
	{
		caption = newCaption;

		glutSetWindowTitle ( caption.c_str () );
	}
	
	void	setShowFps ( bool flag )
	{
		showFps = flag;
	}
	
	float	getDeltaTime () const
	{
		return time - 0.001f * glutGet ( GLUT_ELAPSED_TIME );
	}
	
	float	getTime () const
	{
		return 0.001f * glutGet ( GLUT_ELAPSED_TIME );
	}
	
	void	setTime ()
	{
		time = 0.001f * glutGet ( GLUT_ELAPSED_TIME );
	}
	
	float	getFps () const
	{
		return fps;
	}
	
	void	updateFps ();
							// activate ARB_debug_output callback if supported
	void	setDebugEnabled ( bool debug );
	
							// called by display to readraw window contents
	virtual	void	redisplay () {}
	
	virtual	void	reshape ( int w, int h )
	{
		width  = w;
		height = h;
		
		glViewport ( 0, 0, w, h );
	}
	
					// modifiers  - GLUT_ACTIVE_SHIFT, GLUT_ACTIVE_CTRL, GLUT_ACTIVE_ALT
					// button - GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
	virtual void	mouseClick         ( int button, int state, int modifiers, int x, int y ) {}
	virtual void	mouseWheel         ( int wheel, int modifiers, int x, int y ) {}
	virtual void	mouseMotion        ( int x, int y ) {}
	virtual void	mousePassiveMotion ( int x, int y ) {}
	
	virtual void	keyTyped ( unsigned char key, int /*modifiers*/, int /*x*/, int /*y*/ )
	{
		if ( key == 27 || key == 'q' || key == 'Q' )	//	quit requested
			::exit ( 0 );
	}
	
	virtual void	keyReleased        ( unsigned char c, int modifiers, int x, int y ) {}
	virtual void	specialKey         ( int key, int modifier, int x, int y );
	virtual void	specialKeyUp       ( int key, int modifier, int x, int y );

	virtual	void	idle () 
	{
		glutPostRedisplay ();
	}
	
	void	errorCallback ( GLenum source, GLenum type, GLuint id, 
                            GLenum severity, GLsizei length, const GLchar * message );
	
	static	int	run ()
	{
		glutMainLoop ();

		return 0;
	}
	
	bool	makeScreenshot ( const char * fileName ) const;
	void 	exit ( const char * fmt, ... );
	
	static	void	init ( int& argc, char ** argv, int major = 4, int minor = 3, 
	                       unsigned mode = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH, bool debug = true, int _numSamples = 1 );
};

const char * getGlErrorString ();

#endif
