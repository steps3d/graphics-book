#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	TestWindow : public GlutRotateWindow
{
	Program		program;
	BasicMesh * mesh = nullptr;
	Texture		tex;
	Texture		projMap;
	glm::vec3	lightDir = glm::vec3 ( 1, 1, -1 );
	glm::vec3	projPos  = glm::vec3 ( 0, 0, 5 );
	float		kd  = 0.8f;
	float		ka  = 0.2f;
	glm::mat4	projMat;
	
public:
	TestWindow () : GlutRotateWindow ( 200, 100, 900, 900, "Projective texturing" )
	{	
		mesh = loadMesh ( "../../Models/teapot.3ds", 0.1f );
		
		if ( mesh == nullptr )
			exit ("Error loading mesh" );
		
		std::string	texName = "../../Textures/Fieldstone.dds";

		if ( !tex.load2D ( texName ) )
			exit ( "Error loading texture %s\n", texName.c_str () );

		if ( !projMap.load2D ( "../../Textures/flower.png" ) )
			exit ( "Error loading textures\n" );

		if ( !program.loadProgram ( "projective.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		projMap.bind       ();
		projMap.setWrapAll ( GL_CLAMP_TO_BORDER );
		projMap.unbind     ();
		
		program.bind ();
	
		eye      = glm::vec3 ( 10, 0, 0 );
		
		program.setUniformFloat  ( "kd", kd );
		program.setUniformFloat  ( "ka", ka );
		program.setUniformVector ( "lightDir", lightDir );
		program.setUniformVector ( "eye",      eye );
		program.setUniformVector ( "projPos",  projPos );
		program.setTexture       ( "image",    0 );
		program.setTexture       ( "projMap",  1 );
	}
	
	void redisplay () override
	{
		glm::mat4	mv = getRotation ();
		glm::mat4	bias = glm::mat4( glm::vec4(0.5f,0.0f,0.0f,0.0f),
									  glm::vec4(0.0f,0.5f,0.0f,0.0f),
									  glm::vec4(0.0f,0.0f,0.5f,0.0f),
									  glm::vec4(0.5f,0.5f,0.5f,1.0f) );
		glm::mat4	v = glm::lookAt      ( projPos, glm::vec3 ( 0, 0, 0 ), glm::vec3 ( 0, 1, 0 ) );
		glm::mat4	p = glm::perspective ( glm::radians ( 30.0f ), getAspect (), 0.01f, 20.0f );
		
		projMat = bias * p * v;
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		tex.bind                 ( 0 );
		projMap.bind             ( 1 );
		program.bind             ();
		program.setUniformMatrix ( "mv",      mv );
		program.setUniformMatrix ( "projMat", projMat );

		mesh -> render ();

		program.unbind ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		program.bind             ();
		program.setUniformMatrix ( "proj", getProjection () );
		program.unbind           ();  
	}

	void mouseWheel ( int wheel, int dir, int x, int y ) override
	{
		eye += glm::vec3 ( dir, 0, 0 ) * 0.1f;
		
					// since eye value has changed
		reshape ( getWidth(), getHeight() );
		
		glutPostRedisplay ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	TestWindow	win;
	
	return GlutWindow::run ();
}
