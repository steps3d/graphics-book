#include	"GlutWindow.h"
#include	"Program.h"
#include	"Texture.h"
#include	"VertexArray.h"
#include	"VertexBuffer.h"
#include	"Data.h"
#include	<glm/gtc/matrix_transform.hpp>
#include	<cstdint>
#include	<vector>
#include	<array>
#include	<map>

class	TestWindow : public GlutWindow
{
	struct	Vertex
	{
		glm::vec3	pos;
		glm::vec2	tex;
	};
	
		// AngelCode .fnt format structs and classes
	struct bmchar 
	{
		uint32_t	x, y;
		uint32_t	width;
		uint32_t	height;
		int32_t		xoffset;
		int32_t		yoffset;
		int32_t		xadvance;
		uint32_t	page;
	};

	Program		 program;
	Texture		 fontTex;
	VertexArray  vao;
	VertexBuffer buf;
	VertexBuffer ind;
	size_t       numTris = 0;
	int			 				lineHeight, base, scaleW, scaleH;	// font properties
	std::map<uint32_t, bmchar>	chars;								// font characters properties

public:
	TestWindow () : GlutWindow ( 200, 100, 900, 900, "Distance Field Font" )
	{
		if ( !program.loadProgram ( "sdf.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
/*		
		if ( !fontTex.load2D ( "SDF/ascii~less.png" ) )
			exit ( "Error loading font texture\n" );
		
		if ( !loadFntFile ( "SDF/ascii.fnt") )
			exit ( "Cannot load font file");
*/
		if ( !fontTex.load2D ( "SDF/out.png" ) )
			exit ( "Error loading font texture\n" );
		
		if ( !loadFntFile ( "SDF/font.fnt") )
			exit ( "Cannot load font file");
		
		vao.create  ();
		vao.bind    ();
		buf.create  ();
		buf.bind    ( GL_ARRAY_BUFFER );
		ind.create  ();
		ind.bind    ( GL_ELEMENT_ARRAY_BUFFER );

		program.setAttrPtr ( 0, 3, sizeof ( Vertex ), (void *) 0 );
		program.setAttrPtr ( 1, 2, sizeof ( Vertex ), (void *) sizeof ( glm::vec3 ) );

		buf.unbind     ();
		vao.unbind     ();
	
		program.bind       ();
		program.setTexture ( "fontMap",   0   );
		
		buildVertices ( glm::vec2 ( 0, 0 ), 0.3, "Test !" );
	}

	void redisplay () override
	{
		glm::mat4	mv = glm::rotate ( glm::mat4(1.0f), glm::radians ( 60.0f ), glm::vec3 ( 0,0,1 ) );
		
		glClear     ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable    ( GL_BLEND );
		glBlendFunc ( GL_ONE, GL_ONE );

		fontTex.bind             ();
		program.bind             ();
		program.setUniformMatrix ( "mv",  mv );
		vao.bind                 ();
		
		glDrawElements ( GL_TRIANGLES, numTris * 3, GL_UNSIGNED_INT, nullptr );
		
		vao.unbind     ();
		program.unbind ();
		fontTex.unbind ();
	}

	bool loadFntFile ( const std::string& fntFileName )
	{
		Data		data ( fntFileName );
		std::string	str;
				
		if ( !data.isOk () )
			return false;

		while ( data.getString ( str, '\n' ) )
		{
			size_t		pos = 0;
			std::string	s   = nextWord ( str, pos );
			
			if ( s == "char" )		// get char properties
			{
				uint32_t charid = nextValuePair ( str, pos );
				
				chars [charid].x        = nextValuePair ( str, pos );
				chars [charid].y        = nextValuePair ( str, pos );
				chars [charid].width    = nextValuePair ( str, pos );
				chars [charid].height   = nextValuePair ( str, pos );
				chars [charid].xoffset  = nextValuePair ( str, pos );
				chars [charid].yoffset  = nextValuePair ( str, pos );
				chars [charid].xadvance = nextValuePair ( str, pos );
				chars [charid].page     = nextValuePair ( str, pos );
			}
			else
			if ( s == "common" )	// get font properties
			{
				lineHeight = nextValuePair ( str, pos );
				base       = nextValuePair ( str, pos );
				scaleW     = nextValuePair ( str, pos );
				scaleH     = nextValuePair ( str, pos );
			}
		}
		
		return true;
	}

	void buildVertices ( const glm::vec2& pos, float size, const std::string& text )
	{
		std::vector<Vertex>		vertexBuffer;
		std::vector<uint32_t>	indexBuffer;
		std::array<uint32_t, 6> indices     = { 0,1,2, 2,3,0 };
		uint32_t 				indexOffset = 0;

		float posx = pos.x;
		float posy = pos.y;

		for ( char ch : text )
		{
			bmchar& charInfo = chars[(uint32_t)ch];

			if ( charInfo.width == 0 )
				charInfo.width = base;

			float charw = ((float)(charInfo.width) / (float)base);
			float dimx  = size * charw;
			float charh = ((float)(charInfo.height) / (float)base);
			float dimy  = size * charh;
			float us    = charInfo.x / (float) scaleW;
			float ue    = (charInfo.x + charInfo.width) / (float)scaleW;
			float te    = charInfo.y / (float)scaleH;
			float ts    = (charInfo.y + charInfo.height) / (float)scaleH;
			float xo    = charInfo.xoffset / (float)base;
			//float yo    = charInfo.yoffset / (float)base;

			vertexBuffer.push_back({ { posx + dimx + xo,  posy + dimy, 0.0f }, { ue, te } });
			vertexBuffer.push_back({ { posx + xo,         posy + dimy, 0.0f }, { us, te } });
			vertexBuffer.push_back({ { posx + xo,         posy,        0.0f }, { us, ts } });
			vertexBuffer.push_back({ { posx + dimx + xo,  posy,        0.0f }, { ue, ts } });

			for ( auto& index : indices )
				indexBuffer.push_back(indexOffset + index);
			
			indexOffset += 4;
			posx        += size * ((float)(charInfo.xadvance) / (float)base);
		}
		
		numTris = text.size () * 2;
		
		buf.bind      ( GL_ARRAY_BUFFER );
		buf.resetData ();
		buf.setData   ( vertexBuffer.size () * sizeof ( Vertex ), vertexBuffer.data (), GL_STATIC_DRAW );
		ind.bind      ( GL_ELEMENT_ARRAY_BUFFER );
		ind.resetData ();
		ind.setData   ( indexBuffer.size ()  * sizeof ( uint32_t ), indexBuffer.data (), GL_STATIC_DRAW );
	}
	
private:
	std::string nextWord ( const std::string& str, size_t& pos ) const
	{
		std::string s;
		
		while ( pos < str.length () && str [pos] == ' ' )
			pos++;
		
		while ( pos < str.length () && str [pos] != ' ' )
			s += str [pos++];
		
		return s;
	}
	
	int32_t nextValuePair ( std::string& str, size_t& pos )
	{
		std::string s = nextWord ( str, pos );
		size_t      i = s.find ( '=' );
		
		return std::stoi ( s.substr ( i + 1 ) );
	}
	
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	TestWindow	win;
	
	return GlutWindow::run ();
}
