//
// Simple wrapper class for freeglut
//

#include	<map>
#include	<GL/glew.h>

#ifdef	_WIN32
	#include	<GL/wglew.h>
#else
	#include	<GL/glxew.h>
#endif

#include	"GlutWindow.h"
#include	"TgaImage.h"

#ifndef GLUT_WHEEL_UP_BUTTON
	#define GLUT_WHEEL_UP_BUTTON 0x0003
#endif

#ifndef GLUT_WHEEL_DOWN_BUTTON
	#define GLUT_WHEEL_DOWN_BUTTON 0x0004
#endif

static	std::map<int, GlutWindow *>	windows;		// map of winId -> win address
static 	bool					    init  = false;
static	bool 						debug = false;
static  int				            mode = 0;
static  int				            numSamples = 1;
static	std::string			        appName;

static void	redisplayFunc ()
{
	GlutWindow * window = windows[glutGetWindow()];
	
	window -> updateFps ();
	window -> redisplay ();
	window -> setTime   ();

	glutSwapBuffers ();
}

static void	reshapeFunc ( int w, int h )
{
	GlutWindow * window = windows [glutGetWindow ()];
	
    window -> reshape ( w, h );
}

static	void	idleFunc () 
{
	GlutWindow * window = windows [glutGetWindow ()];
	
	window -> idle ();
}

static void	mouseClickFunc ( int button, int state, int x, int y )
{
    int modifier = glutGetModifiers ();

    if ( button == GLUT_WHEEL_UP_BUTTON ) 
        windows [glutGetWindow()] -> mouseWheel ( 1, 1, x, y );
	else
    if ( button == GLUT_WHEEL_DOWN_BUTTON ) 
        windows [glutGetWindow()] -> mouseWheel ( -1, -1, x, y );
	else
		windows [glutGetWindow()] -> mouseClick ( button, state, modifier, x, y );
}

static	void	mouseMotionFunc ( int x, int y ) 
{
    windows [glutGetWindow()] -> mouseMotion ( x, y );
}

static	void	mousePassiveMotionFunc ( int x, int y )
{
    windows [glutGetWindow()] -> mousePassiveMotion ( x, y );
}

static void wheelFunc ( int button, int dir, int x, int y )
{
	windows [glutGetWindow()] -> mouseWheel ( button, dir, x, y );
}

static	void	keyTypedFunc ( unsigned char ch, int x, int y ) 
{
    int modifier = glutGetModifiers();
	
    windows [glutGetWindow()] -> keyTyped ( ch, modifier, x, y );
}

static void	keyReleasedFunc ( unsigned char ch, int x, int y ) 
{
    int modifier = glutGetModifiers();
	
    windows [glutGetWindow()] -> keyReleased ( ch, modifier, x, y );
}

static void	specialKeyFunc ( int key, int x, int y ) 
{
    int modifier = glutGetModifiers();
	
    windows [glutGetWindow()] -> specialKey ( key, modifier, x, y );
}

static	void	specialKeyReleasedFunc ( int key, int x, int y ) 
{
    int modifier = glutGetModifiers();
	
    windows [glutGetWindow()] -> specialKeyUp ( key, modifier, x, y );
}

const char * stringForSource ( GLenum source )
{
	switch ( source )
	{
		case GL_DEBUG_SOURCE_API_ARB:
			return "API";
			
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
			return "Window system";
			
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
			return "Shader compiler";
			
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
			return "Third party";
			
        case GL_DEBUG_SOURCE_APPLICATION_ARB:
			return "Application";
			
        case GL_DEBUG_SOURCE_OTHER_ARB:
			return "Other";
			
		default:
			return "Source unknown";
	}
}

const char * stringForType ( GLenum type )
{
	switch ( type )
	{
		case GL_DEBUG_TYPE_ERROR_ARB:
			return "Error";
			
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
			return "Deprecated behaviour";
			
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
			return "Undefined behaviour";
			
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
			return "Portability";
			
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
			return "Performance";
			
        case GL_DEBUG_TYPE_OTHER_ARB:
			return "Type other";
			
		default:
			return "Type unknown";
	}
}

const char * stringForSeverity ( GLenum severity )
{
	switch ( severity )
	{
		case GL_DEBUG_SEVERITY_HIGH_ARB:
			return "High";
			
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
			return "Medium";
			
        case GL_DEBUG_SEVERITY_LOW_ARB:
			return "Low";
			
		default:
			return "Severity unknown";
	}
}

static void	CALLBACK _errorCallback ( GLenum source, GLenum type, GLuint id, 
                                     GLenum severity, GLsizei length, const GLchar * message, GLvoid * userParam )
{
	GlutWindow * window = (GlutWindow *) userParam;
	
	if ( window == nullptr )
	{
		printf ( "-----------------------------------------------------------------------------------\n" );
		printf ( "Error:\n\tSource: %s\n\tType: %s\n\tId: %d\n\tSeverity: %s\n\tMessage: %s\n",
				 stringForSource ( source ), stringForType ( type ), id, stringForSeverity ( severity ), message );
	}
	else
		window->errorCallback ( source, type, id, severity, length, message );
}

////////////////////////////////////////////////////////////////////

GlutWindow::GlutWindow ( int xo, int yo, int w, int h, const char * theCaption ) : caption ( theCaption )
{
	glutInitWindowPosition ( xo, yo );
	glutInitWindowSize     ( width = w, height = h );
	
	id              = glutCreateWindow ( caption.c_str() );
	screenshotName  = "screenshot%d.tga";
	screenshotKey   = GLUT_KEY_F1;
	screenshotCount = 1;
	windows [id]    = this;

	if ( !::init )
	{
		glewExperimental = GL_TRUE;
		::init           = true;

		glewInit   ();
		glGetError ();
	}

	glutDisplayFunc       ( redisplayFunc          );
	glutReshapeFunc       ( reshapeFunc            );
	glutIdleFunc          ( idleFunc               );
	glutMouseFunc         ( mouseClickFunc         );
	glutMotionFunc        ( mouseMotionFunc        );
	glutPassiveMotionFunc ( mousePassiveMotionFunc );
	glutKeyboardFunc      ( keyTypedFunc           );
	glutKeyboardUpFunc    ( keyReleasedFunc        );
	glutSpecialFunc       ( specialKeyFunc         );
	glutSpecialUpFunc     ( specialKeyReleasedFunc );
	glutMouseWheelFunc    ( wheelFunc   );
	
	setTime               ();

	if ( !GLEW_VERSION_3_3 )
		exit ( "OpenGL 3.3 not supported.\n" );

	glClearColor ( 0.5, 0.5, 0.5, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glDepthFunc  ( GL_LEQUAL );
	
	setDebugEnabled ( debug );

	if ( numSamples > 1 )
		glEnable ( GL_MULTISAMPLE );
	
	for ( int i = 0; i < 5; i++ )
		frameTime [i] = getTime ();
}

GlutWindow::~GlutWindow ()
{
    glutDestroyWindow ( id );
    glutLeaveMainLoop ();

    windows.erase( id );
}

void	GlutWindow :: init ( int& argc, char ** argv, int major, int minor, unsigned _mode, bool dbg, int _numSamples )
{
	std::string	str ( argv [0] );

#ifdef	_WIN32
	std::string::size_type pos = str.rfind ( '\\' );
#else
	std::string::size_type pos = str.rfind ( '/' );
#endif	
	if ( pos != std::string::npos )
		str = str.substr ( pos + 1 );

#ifdef	_WIN32
	if ( str.length () > 4 )
		if ( str.substr ( str.length () - 4, 4 ) == ".exe" )
			str = str.substr ( 0, str.length () - 4 );
#endif

	appName = str;
	debug = dbg;
	mode  = _mode;
	numSamples = _numSamples;

	if ( numSamples > 1 ) 
	{
		mode |= GLUT_MULTISAMPLE;			
		glutSetOption ( GLUT_MULTISAMPLE, numSamples );
	}

    glutInit               ( &argc, argv );
    glutInitDisplayMode    ( mode );
    glutInitContextVersion ( major, minor );
    glutInitContextProfile ( GLUT_CORE_PROFILE );
    glutInitContextFlags   ( GLUT_FORWARD_COMPATIBLE | (debug ? GLUT_DEBUG : 0 ) );
}

void	GlutWindow :: specialKey ( int key, int modifier, int x, int y )
{
	if ( key == screenshotKey )
	{
		char		buf  [256];
		std::string	str;
		
		sprintf        ( buf, screenshotName.c_str (), screenshotCount++ );

		if ( !appName.empty () )
			str = appName + "_" + buf;
		else
			str = buf;

		makeScreenshot ( str.c_str () );
	}
}

void	GlutWindow :: specialKeyUp ( int key, int modifier, int x, int y )
{
	
}

void	GlutWindow :: updateFps ()
{
	for ( int i = 0; i < 5-1; i++ )
		frameTime [i] = frameTime [i+1];
		
	frameTime [5-1] = getTime ();
	
	fps = 5 / (frameTime [5-1] - frameTime [0] + 0.00001f);		// add EPS to avoid zero division
	frame++;
	
	if ( showFps && (frame % 5 == 0) )			// update FPS every 5'th frame
	{
		char buf [64];
		
		sprintf ( buf, "%5.1f ", fps );
		
		std::string	str ( std::string( buf ) + caption );
		
		glutSetWindowTitle ( str.c_str () );
	}
}

void GlutWindow :: setDebugEnabled ( bool debug )
{
	if ( debug && !GLEW_ARB_debug_output )
		exit ( "ARB_debug_output not supported\n" );
	
	debugEnabled = debug;
	
	glDebugMessageCallbackARB ( (GLDEBUGPROCARB)_errorCallback, this );
}

bool	GlutWindow::makeScreenshot ( const char * fileName ) const
{
	TgaImage	image ( width, height );
	
    uint8_t * ptr = (uint8_t *) malloc ( 3 * width * height );

    if ( ptr == nullptr )
        return false;

    glPixelStorei   ( GL_PACK_ALIGNMENT, 1 );                         // set 1-byte alignment
    glFinish        ();
    glReadBuffer    ( GL_FRONT );
    glReadPixels    ( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, ptr );

	image.setRgbData ( ptr );				// it should do RGB->BGR mapping
	
	free ( ptr );
                                            // now save it as 24-bit uncompressed tga
    return image.writeToFile ( fileName );
}

void GlutWindow::exit ( const char * fmt, ... )
{
	if ( fmt == nullptr )
	{
		::exit ( 1 );
		return;
	}
		
	char	text [1024];
	va_list	ap;

	va_start ( ap, fmt );	
	vsprintf ( text, fmt, ap );	
	va_end   ( ap );
	printf   ( text );
	::exit   ( 1 );
}

void	GlutWindow:: errorCallback ( GLenum source, GLenum type, GLuint id, 
                                     GLenum severity, GLsizei length, const GLchar * message )
{
	printf ( "-----------------------------------------------------------------------------------\n" );
	printf ( "Error:\n\tSource: %s\n\tType: %s\n\tId: %d\n\tSeverity: %s\n\tMessage: %s\n",
			 stringForSource ( source ), stringForType ( type ), id, stringForSeverity ( severity ), message );
 
}

const char * getGlErrorString ()
{
	int error;
	
	if ( (error = glGetError()) != GL_NO_ERROR)
	{
		switch ( error )
		{
		case GL_NO_ERROR:				// no error, return NULL
			return nullptr;
			
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
			
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
			
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
			
		case GL_OUT_OF_MEMORY:
			return  "GL_OUT_OF_MEMORY";
			
		default:
			return "UNKNOWN ERROR";
		}
	}
	
	return nullptr;
}
