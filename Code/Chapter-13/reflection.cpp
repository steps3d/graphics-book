//
// Planar reflection example 
//
// Author: Alexey Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutCameraWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"
#include	"Framebuffer.h"

class	ReflectionWindow : public GlutCameraWindow
{
	Program     program, program2;
	Texture     decalMap;
	Texture     stoneMap;
	Texture     knotMap;
	FrameBuffer fb;
	float	    angle    = 0;
	BasicMesh * box1 = nullptr;
	BasicMesh * box2 = nullptr;
	BasicMesh * knot = nullptr;
	BasicMesh * w    = nullptr;		// water mesh
	glm::mat4   mirrorMat;
	
public:
	ReflectionWindow () : GlutCameraWindow ( 200, 200, 800, 600, "Planar reflection" )
	{
		fb.create ( getWidth (), getHeight (), FrameBuffer::depth24 );
		fb.bind   ();
		
		Texture * image = fb.createColorTexture ( GL_RGBA, GL_RGBA8, GL_CLAMP_TO_EDGE, 0 );
		
		if ( !fb.attachColorTexture ( image ) )
			exit ( "buffer error with color attachment\n");

		if ( !fb.isOk () )
			exit ( "Error with framebuffer\n" );
						
		fb.unbind ();

		if ( !program.loadProgram ( "reflection-1.glsl" ) )
			exit ( "Error loading shader: %s\n", program.getLog ().c_str () );
		
		program.bind       ();
		program.setTexture ( "image", 0 );
		program.unbind     ();
		
		if ( !program2.loadProgram ( "reflection-2.glsl" ) )
			exit ( "Error loading shader: %s\n", program2.getLog ().c_str () );
		
		program2.bind       ();
		program2.setTexture ( "image", 0 );
		program2.unbind     ();
		
		box1 = createBox  ( glm::vec3 ( -6, -2, -6 ),    glm::vec3 ( 12, 5, 12 ) );
		box2 = createBox  ( glm::vec3 ( -1.5, 1, -0.5 ), glm::vec3 ( 1,  2,  2 ) );
		knot = createKnot ( 1, 4, 120, 30 );
		w    = createQuad ( glm::vec3 ( -6, 0, -6 ), glm::vec3 ( 12, 0, 0 ), glm::vec3 ( 0, 0, 12 ) );

		decalMap.load2D ( "../../Textures/oak.jpg" );
		stoneMap.load2D ( "../../Textures/block.jpg" );
		knotMap.load2D  ( "../../Textures/Oxidated.jpg" );
	}
	
	void redisplay () override
	{
		mv   = getModelView  ();
		proj = getProjection ();

		renderToBuffer ();
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		mv   = getModelView  ();
		proj = getProjection ();
		
		program.bind             ();
		program.setUniformMatrix ( "proj",  proj );
		program.setUniformMatrix ( "mv",    mv   );
		
		displayBoxes ();
		
		program.unbind ();

		renderWater ();
	}
	
	void	idle () override
	{
		angle = 4 * getTime ();

		glutPostRedisplay ();
	}

private:	
	void displayBoxes ()
	{
		decalMap.bind ();
		box1 -> render ();
		decalMap.unbind ();
		stoneMap.bind ();
		box2 -> render ();
		stoneMap.unbind ();

		glm::mat4 m = mv * glm::translate ( glm::mat4 (1), glm::vec3 ( 2, 0, 1 ) ) * 
		                   glm::rotate    ( glm::mat4 (1), angle * 0.3f,  glm::vec3 ( 1,0,0 ) ) * 
						   glm::rotate    ( glm::mat4 (1), angle * 0.07f, glm::vec3 ( 0,1,0 ) ) * 
						   glm::scale     ( glm::mat4 (1), glm::vec3 ( 0.3f ) );
		
		program.setUniformMatrix ( "mv",  m );

		knotMap.bind ();
		
		knot -> render ();
		
		knotMap.unbind ();
	}

	void    renderToBuffer ()
	{
		fb.bind ();
		
		glClearColor ( 0, 0, 1, 1 );
		glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glm::vec3 up    = camera.getUpDir   ();
		glm::vec3 view  = camera.getViewDir ();
		glm::vec3 pos   = camera.getPos     ();

		up.y   = -up.y;			// reflect camera vectors
		view.y = -view.y;
		pos.y  = -pos.y;
		
		mv        = glm::lookAt ( pos, pos + view, up );
		mirrorMat = glm::translate ( glm::mat4 (1), glm::vec3 ( 0.5, 0.5, 0 ) ) * 
		            glm::scale     ( glm::mat4 (1), glm::vec3 ( 0.5, 0.5, 1 ) ) * proj * mv;

		program.bind             ();
		program.setUniformMatrix ( "proj",  getProjection () );
		program.setUniformMatrix ( "mv",    mv );
		
		displayBoxes ();
		
		fb.unbind ();
	}

	void renderWater ()
	{
		fb.getColorBuffer () -> bind ();
		
		program2.bind             ();
		program2.setUniformMatrix ( "proj",  proj );
		program2.setUniformMatrix ( "mv",    mv   );
		program2.setUniformMatrix ( "mirrorMat", mirrorMat );
		program2.setUniformFloat  ( "time",  getTime () );
		
		w  -> render ();
		
		program2.unbind ();

		fb.getColorBuffer () -> unbind ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	ReflectionWindow	win;
	
	return GlutWindow::run ();
}
