//
// Shadow mapping example 
//
// Author: Alexey V. Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"
#include	"Framebuffer.h"

class	ShadowWindow : public GlutRotateWindow
{
	FrameBuffer	fb;
	Program		program, program2;
	Texture	  * shadowMap = nullptr;
	Texture 	decalMap;
	Texture 	stoneMap;
	BasicMesh * mesh1 = nullptr;
	BasicMesh * mesh2 = nullptr;
	glm::vec3	light = glm::vec3 ( 7, 0, 7 );
	glm::mat4   shadowMat;

public:
	ShadowWindow () : GlutRotateWindow ( 100, 100, 900, 900, "Shadow Mapping" ) 
	{
		fb.create             ( 512, 512, 0 );
		fb.bind               ();
		fb.attachDepthTexture ( shadowMap = fb.createColorTexture ( GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_CLAMP_TO_BORDER, FrameBuffer::filterNearest ) );
		
		if ( !fb.isOk () )
			exit ( "Error with fb\n" );
			
		fb.unbind ();
		
		if ( !program.loadProgram ( "decal.glsl" ) )
			exit ( "Error loading shader: %s\n", program.getLog ().c_str () );
		
		program.bind       ();
		program.setTexture ( "image", 0 );
		program.unbind     ();
		
		if ( !program2.loadProgram ( "shadow.glsl" ) )
			exit ( "Error loading shader: %s\n", program2.getLog ().c_str () );
		
		program2.bind       ();
		program2.setTexture ( "image",     0 );
		program2.setTexture ( "shadowMap", 1 );
		program2.unbind     ();

		mesh1 = createQuad  ( glm::vec3 ( -6, -6, -2 ), glm::vec3 ( 12, 0, 0 ), glm::vec3 ( 0, 12, 0 ) );
		mesh2 = createKnot ( 0.5, 2, 90, 50 );

		decalMap.load2D ( "../../Textures/wood.png" );
		stoneMap.load2D ( "../../Textures/brick.tga" );
	}
	
	void redisplay () override
	{
		renderToShadowMap ();
	
		glm::mat4	mv = getRotation ();

		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		program2.bind             ();
		program2.setUniformMatrix ( "mv",    mv   );
		program2.setUniformMatrix ( "nm",  normalMatrix ( mv ) );
		program2.setUniformMatrix ( "shadowMat",  shadowMat );
		program2.setUniformVector ( "light", light );

		decalMap.bind   ( 0 );
		shadowMap->bind ( 1 );
		mesh1->render   ();
		
		stoneMap.bind   ( 0 );
		shadowMap->bind ( 1 );
		mesh2->render   ();
		
		program2.unbind ();
	}
	
	void	keyTyped ( unsigned char key, int modifiers, int x, int y ) override
	{
		GlutRotateWindow :: keyTyped ( key, modifiers, x, y );

		if ( key == ' ' )
		{
			shadowMap -> bind       ();
			shadowMap -> saveToFile ( "depth.tga", GL_DEPTH_COMPONENT );
			shadowMap -> unbind     ();
		}
	}

	void renderToShadowMap ()
	{
		glm::mat4 proj   = glm::perspective ( glm::radians(90.0f), (float)shadowMap->getWidth()/(float)shadowMap->getHeight(), 0.01f, 20.0f );
		glm::mat4 mv     = glm::lookAt      ( light, glm::vec3 ( 0, 0, 0 ), glm::vec3 ( 0, 0, 1 ) );
		
		shadowMat = proj * mv;
		
		program.bind             ();
		program.setUniformMatrix ( "proj",  shadowMat );
		program.setUniformMatrix ( "mv",    getRotation () );
		program.setUniformMatrix ( "nm",    normalMatrix ( mv ) );
		program.setUniformVector ( "light", light );
		
		fb.bind ();

		glClear ( GL_DEPTH_BUFFER_BIT );
		
		decalMap.bind   ();
		mesh1->render   ();
		decalMap.unbind ();
		
		stoneMap.bind  ();
		mesh2->render  ();
		stoneMap.unbind ();
		
		fb.unbind      ();
		program.unbind ();  
		
		glFinish ();
	}
	
	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		program2.bind             ();
		program2.setUniformMatrix ( "proj", getProjection () );
		program2.unbind           ();  
	}

	void mouseWheel ( int wheel, int dir, int x, int y ) override
	{
		eye += glm::vec3 ( dir, dir, dir ) * 0.5f;

		reshape ( getWidth (), getHeight () );
		
		glutPostRedisplay ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	ShadowWindow	win;
	
	return GlutWindow::run ();
}
