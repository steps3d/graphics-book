#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	TestWindow : public GlutRotateWindow
{
	Program	    program;
	BasicMesh * mesh = nullptr;
	Texture	    albedo, metalness, normal, roughness;
	glm::vec3   lightDir   = glm::vec3 ( -1, 1, 1 );;
	glm::vec3   lightColor = glm::vec3 ( 4.0f );
	
public:
	TestWindow () : GlutRotateWindow ( 200, 50, 900, 900, "Physically Based Rendering" )
	{	
		if ( !program.loadProgram ( "pbr.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
//		mesh = loadMesh ( "PBR/source/m1911_finalFire.fbx", 0.1f );
		mesh = loadMesh ( "PBR/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX", 0.1f );
		
		if ( mesh == nullptr )
			exit ("Error loading mesh\n" );
		
//		std::string	albedoName    = "PBR/textures/m1911_color.png";
		std::string	albedoName    = "PBR/Cerberus_by_Andrew_Maximov/Textures/Cerberus_A.tga";
		std::string	metalnessName = "PBR/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.tga";
		std::string	normalName    = "PBR/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.tga";
		std::string	roughnessName = "PBR/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.tga";

		if ( !albedo.load2D ( albedoName ) )
			exit ( "Error loading texture %s\n", albedoName.c_str () );

		if ( !metalness.load2D ( metalnessName ) )
			exit ( "Error loading texture %s\n", metalnessName.c_str () );

		if ( !normal.load2D ( normalName ) )
			exit ( "Error loading texture %s\n", normalName.c_str () );

		if ( !roughness.load2D ( roughnessName ) )
			exit ( "Error loading texture %s\n", roughnessName.c_str () );
	
		eye      = glm::vec3 ( -7, 0, 0 );
		
		program.bind             ();	
		program.setUniformVector ( "lightDir",      lightDir );
		program.setUniformVector ( "lightColor",    lightColor );
		program.setUniformVector ( "eye",           eye );
		program.setTexture       ( "albedoMap",     0 );
		program.setTexture       ( "metallnessMap", 1 );
		program.setTexture       ( "normalMap",     2 );
		program.setTexture       ( "roughnessMap",  3 );
	}
	
	void redisplay () override
	{
		glm::mat4	mv = getRotation  ();
		glm::mat3	nm = normalMatrix ( mv );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		albedo.bind              ( 0 );
		metalness.bind           ( 1 );
		normal.bind              ( 2 );
		roughness.bind           ( 3 );
		program.bind             ();
		program.setUniformMatrix ( "mv", mv );
		program.setUniformMatrix ( "nm",  nm );

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
		eye += glm::vec3 ( dir, 0, 0 ) * 0.3f;
		
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
