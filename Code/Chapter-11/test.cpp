#include	"GlutWindow.h"

class	TestWindow : public GlutWindow
{
public:
	TestWindow () : GlutWindow ( 100, 200, 500, 500, "Test" ) {}

	void	redisplay () override
	{
		glClear ( GL_COLOR_BUFFER_BIT );
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );

	TestWindow win;

	return GlutWindow::run ();
}
