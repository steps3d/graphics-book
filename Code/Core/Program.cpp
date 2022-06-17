//
// Class to encapsulate GLSL program and shader objects and working with them
//
// Author: Alex V. Boreskoff <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	<glm/gtc/type_ptr.hpp>
#include	"Data.h"
#include	"glUtilities.h"
#include	"Program.h"
#include	<malloc.h>
#include	<map>
#include	"stringUtils.h"

GLuint	Program::boundProgram = 0;		// not anyone bound yet

Program :: Program ()
{
	program           = 0;
	vertexShader      = 0;
	fragmentShader    = 0;
	geometryShader    = 0;
	tessControlShader = 0;
	tessEvalShader    = 0;
	computeShader     = 0;
	ok                = false;
	linkRequired      = true;
	separate          = false;
}

Program :: ~Program ()
{
	destroy ();
}

bool	Program :: loadProgram ( const std::string& file )
{
	Data data ( file );
	
	fileName = file;
	
	if ( !data.isOk () || data.getLength () < 1 )
	{
		log += "Error loading shader from ";
		log += fileName;
		log += "\n";
		
		return false;
	}

	return loadProgram ( &data );
}

bool	Program :: loadProgram ( Data * data )
{
	glGetError ();		// clear OpenGL error

	struct Chunk
	{
		const char * name;
		GLenum		 type;
		//int			 start, end;
		std::string	 body;
	};
	
	Chunk	info [] = 
	{
		{ "vertex",      GL_VERTEX_SHADER          },
		{ "fragment",    GL_FRAGMENT_SHADER        },
		{ "geometry",    GL_GEOMETRY_SHADER        },
		{ "tesscontrol", GL_TESS_CONTROL_SHADER    },
		{ "tesseval",    GL_TESS_EVALUATION_SHADER },
		{ "compute",     GL_COMPUTE_SHADER         }
	};
	
	std::string	s, body;
	int			start = 0, end = 0;
	int			type = -1;
	
	log += "Started loading composite shader from ";
	log += data -> getFileName ();
	log += "\n";

	while ( data -> getString ( s, '\n' ) )
	{
													// handle #include
		if ( stringTrim  ( s ).substr ( 0, 8 ) == "#include" )
		{
			std::string	file = stringDequote ( stringTrim ( stringTrim  ( s ).substr ( 8 ) ) );
			Data		d ( file );

			if ( !d.isOk () )
			{
				log += "Cannot open file \"" + file + "\"";
				
				return false;
			}

			body += (const char *) d.getPtr ();
			
			continue;
		}											// not a start of a new shader
		else if ( s.empty () || s.length () < 2 || s [0] != '-' || s [1] != '-' )
		{
			end   = data -> getPos ();
			body += s;
			body += '\n';

			continue;
		}
			
		s = stringLower ( stringTrim ( s.substr ( 2 ) ) );
		
		if ( start < end && type != -1 )			// there is shader
		{
			//info [type].start = start;
			//info [type].end   = end;
			info [type].body  = body;
		}
		
		start = data -> getPos ();
		body  = "";
		end   = start;
		type  = -1;
		
		for ( size_t i = 0; i < sizeof ( info ) / sizeof ( info [0] ); i++ )
			if ( s == info [i].name )
			{
				type = (int)i;
				break;
			}
	}
	
	if ( start < end && type != -1 )				// process last shader
	{
		//info [type].start = start;
		//info [type].end   = end;
		info [type].body  = body;
		start             = data -> getPos ();
		end               = start;
	}
	
						// now load all chunks
	for ( size_t i = 0; i < sizeof ( info ) / sizeof ( info [0] ); i++ )
		if ( !info [i].body.empty () )	//info [i].start < info [i].end )
		{
			//Data	d    ( data -> getPtr ( info [i].start ), info [i].end - info [i].start );
			Data	data ( (void *)info [i].body.c_str (), info [i].body.length () + 1 );		// XXX - m.b. strdup ?
		
			if ( !loadShaderOfType ( &data, info [i].type ) )
				return false;
		}
	
	return relink ();
}

bool	Program :: loadProgramFromString ( const std::string& source )
{
	Data	data ( (void *)source.c_str (), source.length () + 1 );		// XXX - m.b. strdup ?
	
	return loadProgram ( &data );
}

						// load source and compile it into existing shader
bool    Program :: loadShader ( GLuint shader, Data * data )
{
	const char * body = (const char *) data -> getPtr ( 0 );
	const GLint  len  = data -> getLength ();
	GLint        compileStatus;

	glShaderSource ( shader, 1, &body,  &len );

                                        // compile the shader
	glCompileShader ( shader );

              // check for OpenGL errors
	if ( glGetError () != GL_NO_ERROR )
		return false;

	glGetShaderiv ( shader, GL_COMPILE_STATUS, &compileStatus );

	loadShaderLog ( shader );

	return compileStatus != 0;
}

						// create and load shader of specific type
bool	Program :: loadShaderOfType ( const char * file, GLenum type )
{
	Data * data = new Data ( file );
	
	fileName = file;
	
	if ( !data -> isOk () || data -> getLength () < 1 )
	{
		log += "Error loading shader from ";
		log += fileName;
		log += "\n";
		
		delete data;
		
		return false;
	}
	
	bool	result = loadShaderOfType ( data, type );
	
	delete data;
	
	return result;
}

						// create and load shader of specifi type
bool	Program :: loadShaderOfType ( Data * data, GLenum type )
{
    if ( program == 0 )
        program = glCreateProgram ();

                                // check for errors
              // check for OpenGL errors
    if ( glGetError () != GL_NO_ERROR )
        return false;

    GLuint	shader = glCreateShader ( type );
	
	switch ( type )
	{
		case GL_VERTEX_SHADER:
			log += "Loading vertex shader.\n";
			vertexShader = shader;
			break;	
			
		case GL_FRAGMENT_SHADER:
			log += "Loading fragment shader.\n";
			fragmentShader = shader;
			break;	
			
		case GL_GEOMETRY_SHADER:
			log += "Loading geometry shader.\n";
			geometryShader = shader;
			break;	
			
		case GL_TESS_CONTROL_SHADER:
			log += "Loading tesselation control shader.\n";
			tessControlShader = shader;
			break;	
			
		case GL_TESS_EVALUATION_SHADER:
			log += "Loading tesselation evaluation shader.\n";
			tessEvalShader = shader;
			break;	
					
		case GL_COMPUTE_SHADER:
			log += "Loading compute shader.\n";
			computeShader = shader;
			break;	
					
		default:
			log += "Unknown shader type.\n";
			return false;
	}
	
						// try to load and compile it
	if ( !loadShader ( shader, data ) )
		return false;
		
	glAttachShader ( program, shader );

	linkRequired = true;
	
	return true;
}

bool    Program :: loadShaders ( const char * vertexFileName, const char * fragmentFileName )
{
	ok = false;

	Data * vertexData = new Data ( vertexFileName );

	if ( vertexData == nullptr )
	{
		log += "Cannot open \"";
		log += vertexFileName;
		log += "\"\n";
	}
	else
	if ( !vertexData -> isOk () || vertexData -> isEmpty () )
	{
		log += "Error loading vertex shader ";
		log += vertexFileName;
		log += "\n";

		delete vertexData;

		vertexData = nullptr;
	}

	Data * fragmentData = new Data ( fragmentFileName );

	if ( fragmentData == nullptr )
	{
		log += "Cannot open \"";
		log += fragmentFileName;
		log += "\"\n";
	}
	else
	if ( !fragmentData -> isOk () || fragmentData -> isEmpty () )
	{
		log += "Error loading fragment shader ";
		log += fragmentFileName;
		log += "\n";

		delete fragmentData;

		fragmentData = nullptr;
	}
	
	bool	result = loadShaders ( vertexData, fragmentData );

	if ( fragmentData == nullptr && fragmentFileName != nullptr )		// file was specified but failed to open
		result = false;
		
	delete vertexData;
	delete fragmentData;

	return result;
}

bool    Program :: loadShaders ( Data * vertexShaderData, Data * fragmentShaderData )
{
	ok = false;
                                // create a vertex shader object and a fragment shader object
	if ( vertexShaderData != nullptr )
	{
	    log += "Loading vertex shader\n";
		
		if ( !loadShaderOfType ( vertexShaderData, GL_VERTEX_SHADER ) )
			return false;
	}

	if ( fragmentShaderData != nullptr )
	{
	    log += "Loading fragment shader\n";
		
		if ( !loadShaderOfType ( fragmentShaderData, GL_FRAGMENT_SHADER ) )
			return false;
	}

	return relink ();
}

bool	Program :: loadSeparate ( GLenum type, Data * data )
{
	if ( !data -> isOk () || data -> getLength () < 1 )
	{
		log += "Error loading separate shader\n";
		
		return false;
	}

		// now create an array of zero-terminated strings
	const char * body  = (const char *) data -> getPtr ( 0 );
	GLint		 len   = data -> getLength ();
	char       * buf   = (char *) malloc ( len + 1 );
	
	memcpy ( buf, body, len );
	buf [len] = '\0';
	
	program      = glCreateShaderProgramv ( type, 1, (const char **)&buf );
	separate     = true;
	linkRequired = false;
	ok           = true;
	
	loadProgramLog ( program );

	free ( buf );
	
	return program != 0;
}

bool	Program :: loadSeparate ( GLenum type, const std::string& file )
{
	Data * data = new Data ( fileName );

	fileName = file;
	
	if ( data == nullptr )
	{
		log += "Cannot open \"";
		log += fileName;
		log += "\"\n";
		
		return false;
	}

	bool	res = loadSeparate ( type, data );
	
	delete data;
	
	return res;
}

bool	Program :: getLinkStatus () const
{
	GLint   linked;

	glGetProgramiv ( program, GL_LINK_STATUS, &linked );

	return linked != 0;
}

bool	Program :: relink ()
{
	GLint   linked;

	log += "Linking programs\n";

	if ( GLEW_ARB_get_program_binary )
		glProgramParameteri ( program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );
	
                                // link the program object and print out the info log
	glLinkProgram ( program );

	if ( glGetError () != GL_NO_ERROR )
		return false;

	glGetProgramiv ( program, GL_LINK_STATUS, &linked );

	loadProgramLog ( program );

	if ( !linked )
		return false;
	
	log += "Link successfull\n";

	linkRequired = false;
	
	return ok = true;
}

Data  * Program :: getBinary ( GLenum * binaryFormat ) const
{
	if ( program == 0 || !getLinkStatus () )
		return nullptr;
		
	if ( !GLEW_ARB_get_program_binary )
		return nullptr;

	GLint   binaryLength;
	void  * binary;

	glGetProgramiv ( program, GL_PROGRAM_BINARY_LENGTH, &binaryLength );
	
	if ( binaryLength < 1 )
		return nullptr;
		
	binary = (GLvoid * )malloc ( binaryLength );
	
	if ( binary == nullptr )
		return nullptr;
		
	glGetProgramBinary ( program, binaryLength, nullptr, binaryFormat, binary );
		
	return new Data ( binary, binaryLength );
}

bool	Program :: saveBinary ( const char * fileName, GLenum * binaryFormat ) const
{
	Data * data = getBinary ( binaryFormat );
	
	if ( data == nullptr )
		return false;
		
	data -> saveToFile ( fileName );
	
	delete data;
	
	return true;
}

bool	Program :: loadBinary ( Data * data, GLenum binaryFormat )
{
	if ( program == 0 )
		return false;
		
	if ( !GLEW_ARB_get_program_binary )
		return false;

	if ( data == nullptr || data -> getLength () < 1 )
		return false;
		
	glProgramBinary ( program, binaryFormat, data -> getPtr (), data -> getLength () );

	return getLinkStatus ();
}

bool	Program :: loadBinary ( const char * fileName, GLenum binaryFormat )
{
	if ( !GLEW_ARB_get_program_binary )
		return false;

	Data * data = new Data ( fileName );
	bool   res  = loadBinary ( data, binaryFormat );
	
	delete data;
	
	return res;
}

Program& Program :: bind ()
{
	glUseProgram ( program );

	boundProgram = program;
	
	return *this;
}

Program& Program :: unbind ()
{
	assert ( boundProgram == program );

	glUseProgram ( 0 );

	boundProgram = 0;
	
	return *this;
}

bool    Program :: checkGlError ()
{
	bool         retCode = true;
	const char * err     = getGlErrorString ();

	if ( err == nullptr )
    		return retCode;

	if ( err != nullptr )
	{
		glError = err;
		log    += glError;
	}
	
	return false;
}

void    Program :: destroy ()
{
	glDeleteProgram ( program           );                  // it will also detach shaders
	glDeleteShader  ( vertexShader      );
	glDeleteShader  ( fragmentShader    );
	glDeleteShader  ( geometryShader    );
	glDeleteShader  ( tessControlShader );
	glDeleteShader  ( tessEvalShader    );
	glDeleteShader  ( computeShader     );

	program           = 0;
	vertexShader      = 0;
	fragmentShader    = 0;
	geometryShader    = 0;
	tessControlShader = 0;
	tessEvalShader    = 0;
	computeShader     = 0;
	ok                = false;
	linkRequired      = true;
}

void    Program :: loadProgramLog ( GLuint object )
{
	GLint       logLength     = 0;
	GLsizei     charsWritten  = 0;
	char   buffer [2048];
	char * infoLog;

	glGetProgramiv ( object, GL_INFO_LOG_LENGTH, &logLength );

	if ( glGetError () != GL_NO_ERROR )          // check for OpenGL errors
		return;

	if ( logLength < 1 )
		return;
                                    // try to avoid allocating buffer
	if ( logLength > (int) sizeof ( buffer ) )
	{
		infoLog = (char*) malloc ( logLength );

		if ( infoLog == nullptr )
		{
			log = "ERROR: Could not allocate log buffer";

			return;
		}
	}
	else
		infoLog = buffer;

	glGetProgramInfoLog ( object, logLength, &charsWritten, infoLog );

	log += infoLog;

	if ( infoLog != buffer )
		free ( infoLog );
}

void    Program :: loadShaderLog ( GLuint object )
{
	GLint       logLength     = 0;
	GLsizei     charsWritten  = 0;
	char   buffer [2048];
	char * infoLog;

	glGetShaderiv ( object, GL_INFO_LOG_LENGTH, &logLength );

	if ( glGetError () != GL_NO_ERROR )          // check for OpenGL errors
        return;

	if ( logLength < 1 )
		return;
                                    // try to avoid allocating buffer
	if ( logLength > (int) sizeof ( buffer ) )
	{
		infoLog = (char*) malloc ( logLength );

		if ( infoLog == nullptr )
		{
			log = "ERROR: Could not allocate log buffer";

			return;
		}
	}
	else
		infoLog = buffer;

	glGetShaderInfoLog ( object, logLength, &charsWritten, infoLog );

	log += infoLog;

	if ( infoLog != buffer )
		free ( infoLog );
}

bool    Program :: setUniformVector ( const char * name, const glm::vec4& value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniform4fv ( loc, 1, glm::value_ptr(value) );

	return true;
}

bool    Program :: setUniformVector  ( int loc, const glm::vec4& value )
{
	assert ( boundProgram == program );

	glUniform4fv ( loc, 1, glm::value_ptr(value) );

	return true;
}

bool    Program :: setUniformVector ( const char * name, const glm::vec3& value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniform3fv ( loc, 1, glm::value_ptr(value) );

	return true;
}

bool    Program :: setUniformVector  ( int loc, const glm::vec3& value )
{
	assert ( boundProgram == program );

	glUniform3fv ( loc, 1, glm::value_ptr(value) );

	return true;
}

bool    Program :: setUniformVector ( const char * name, const glm::vec2& value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniform2fv ( loc, 1, glm::value_ptr(value) );

	return true;
}

bool    Program :: setUniformVector  ( int loc, const glm::vec2& value )
{
	assert ( boundProgram == program );

	glUniform2fv ( loc, 1, glm::value_ptr(value) );

	return true;
}

bool    Program :: setUniformFloat ( const char * name, float value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniform1f ( loc, value );

	return true;
}

bool    Program :: setUniformFloat ( int loc, float value )
{
	assert ( boundProgram == program );

	glUniform1f ( loc, value );

	return true;
}

bool    Program :: setUniformInt ( const char * name, int value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniform1i ( loc, value );

	return true;
}

bool    Program :: setUniformInt ( int loc, int value )
{
	assert ( boundProgram == program );

	glUniform1i ( loc, value );

	return true;
}

bool    Program :: setUniformUInt ( const char * name, GLuint value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniform1ui ( loc, value );

	return true;
}

bool    Program :: setUniformUInt ( int loc, GLuint value )
{
	assert ( boundProgram == program );

	glUniform1ui ( loc, value );

	return true;
}

bool    Program :: setUniformMatrix  ( const char * name, const glm::mat4& value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniformMatrix4fv ( loc, 1, GL_FALSE/*GL_TRUE*/, glm::value_ptr(value) );

	return true;
}

bool    Program :: setUniformMatrices  ( const char * name, const glm::mat4 * value, int n )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniformMatrix4fv ( loc, n, GL_FALSE /*GL_TRUE*/, (const GLfloat *) value );

	return true;
}

bool    Program :: setUniformVectors ( const char * name, const glm::vec4 * values, int n )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniform4fv ( loc, n, (const GLfloat *) values );

	return true;
}


bool    Program :: setUniformMatrix  ( const char * name, const glm::mat3& value )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniformMatrix3fv ( loc, 1, GL_FALSE, glm::value_ptr(value) );		// require matrix transpose

	return true;
}

bool    Program :: setUniformMatrix  ( const char * name, float value [16] )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return false;

	glUniformMatrix4fv ( loc, 1, GL_FALSE, value );

	return true;
}

int     Program :: locForUniformName ( const char * name )
{
	return glGetUniformLocation ( program, name );
}

int	Program :: indexForUniformBlock ( const char * name ) const
{
	return glGetUniformBlockIndex ( program, name );
}

int	Program :: uniformBlockSize ( int blockIndex ) const
{
	int	size;
	
	glGetActiveUniformBlockiv ( program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &size );
	
	return size;
}

int	Program :: uniformBlockBinding ( int blockIndex ) const
{
	int	binding;
	
	glGetActiveUniformBlockiv ( program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &binding );
	
	return binding;
}

int	Program :: uniformBlockActiveUniforms ( int blockIndex ) const
{
	int	num;
	
	glGetActiveUniformBlockiv ( program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num );
	
	return num;
}

bool	Program :: getUniformBlockVar ( int blockIndex, const char * varName, int& offs, int& size )
{
	GLuint	varIndex;
	
	glGetUniformIndices ( program, 1, &varName, &varIndex );
	
	if ( varIndex == GL_INVALID_INDEX )
		return false;
		
	glGetActiveUniformsiv ( program, 1, &varIndex, GL_UNIFORM_SIZE,   &size );
	glGetActiveUniformsiv ( program, 1, &varIndex, GL_UNIFORM_OFFSET, &offs );
	
	return true;
}

void	Program :: bindBufferToIndex ( int uniformBlockIndex, int bindingIndex )
{
		// assign a binding point to an active uniform block
	 glUniformBlockBinding ( program, (GLuint)uniformBlockIndex, (GLuint)bindingIndex );
}

void	Program :: bindBufferToIndex ( const char * uniformBlockName, int bindingIndex )
{
	GLuint	blockIndex = glGetProgramResourceIndex ( program, GL_UNIFORM_BLOCK, uniformBlockName );

		// assign a binding point to an active uniform block
	 glUniformBlockBinding ( program, blockIndex, (GLuint)bindingIndex );
}

glm::vec4    Program :: getUniformVector ( const char * name )
{
	assert ( boundProgram == program );

	float   values [4];

	int loc = glGetUniformLocation ( program, name );

	if ( loc < 0 )
		return glm::vec4 ( 0, 0, 0, 0 );

	glGetUniformfv ( program, loc, values );

	return glm::vec4 ( values [0], values [1], values [2], values [3] );
}

bool    Program :: setTexture ( const char * name, int texUnit )
{
	assert ( boundProgram == program );

	int loc = glGetUniformLocation ( program, name );

	if ( loc == -1 )
		return false;

	glUniform1i ( loc, texUnit );

	return true;
}

bool    Program :: setTexture ( int loc, int texUnit )
{
	assert ( boundProgram == program );

	if ( loc < 0 )
		return false;

	glUniform1i ( loc, texUnit );

	return true;
}

bool	Program :: bindFragOut ( const char * name, int no )
{
	glBindFragDataLocation ( program, no, name );
	
	linkRequired = true;
	
	return true;
}	

bool	Program :: bindFragOutIndexed ( const char * name, int no, int index )
{
	glBindFragDataLocationIndexed ( program, no, index, name );
	
	linkRequired = true;
	
	return true;
}	

int     Program :: indexForAttrName ( const char * name )
{
	return glGetAttribLocation ( program, name );
}

bool	Program :: setAttrPtr ( const char * name, int numComponents, GLsizei stride, void * ptr, GLenum type, bool normalized )
{
	int	loc = indexForAttrName ( name );

	if ( loc < 0 )
		return false;
		
	glVertexAttribPointer ( loc, 					// index
							numComponents, 			// number of values per vertex
							type, 					// type (GL_FLOAT)
							normalized ? GL_TRUE : GL_FALSE,
							stride, 				// stride (offset to next vertex data)
							(const GLvoid*) ptr );
		
	glEnableVertexAttribArray ( loc );
	
	return true;
}

bool	Program :: setAttrPtr ( int loc, int numComponents, GLsizei stride, void * ptr, GLenum type, bool normalized )
{
	if ( loc < 0 )
		return false;
		
	glVertexAttribPointer ( loc, 					// index
							numComponents, 			// number of values per vertex
							type, 					// type (GL_FLOAT)
							normalized ? GL_TRUE : GL_FALSE,
							stride, 				// stride (offset to next vertex data)
							(const GLvoid*) ptr );
		
	glEnableVertexAttribArray ( loc );
	
	return true;
}

void	Program :: transformFeedbacksVars ( const char * names, GLenum mode )
{
	char	buf      [2048];				// hope this will be enough
	char  * varyings [128];
	size_t	count = 0;
	
	for ( char * ptr = strtok ( strcpy ( buf, names ), " ,;" ); ptr; ptr = strtok ( nullptr, " ,;" ) )
		varyings [count++] = ptr;

	glTransformFeedbackVaryings ( program, count, (const GLchar **)varyings, mode );
	
	linkRequired = true;

	relink ();
}

int	Program :: getGeometryInputType () const
{
	int	type;
	
	glGetProgramiv ( program, GL_GEOMETRY_INPUT_TYPE, &type );
	
	return type;
}

int	Program :: getGeometryOutputType () const
{
	int	type;
	
	glGetProgramiv ( program, GL_GEOMETRY_OUTPUT_TYPE, &type );
	
	return type;
}

int	Program :: getGeometryVerticesOut () const
{
	int	num;
	
	glGetProgramiv ( program, GL_GEOMETRY_VERTICES_OUT, &num );
	
	return num;
}

void	Program :: setTessDefaultOuterLevel ( const float v [4] )
{
	if ( isTesselationShaderSupported () )
		glPatchParameterfv ( GL_PATCH_DEFAULT_OUTER_LEVEL, v );
}

void	Program :: setTessDefaultInnerLevel ( const float v [2] )
{
	if ( isTesselationShaderSupported () )
		glPatchParameterfv ( GL_PATCH_DEFAULT_INNER_LEVEL, v );
}

int	Program :: tessControlVerticesOut () const
{
	int	n = 0;

	glGetProgramiv ( program, GL_TESS_CONTROL_OUTPUT_VERTICES, &n );

	return n;
}

bool    Program :: isSupported ()
{
	return GLEW_ARB_shading_language_100 && GLEW_ARB_shader_objects && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader;
}

bool	Program :: isGeometryShaderSupported ()
{
	return GLEW_EXT_geometry_shader4 != 0;
}

bool	Program :: isTesselationShaderSupported ()
{
	return GLEW_ARB_tessellation_shader != 0;
}

std::string  Program :: version ()
{
	glGetError ();		// clear old error status, if any

	const char * slVer = (const char *) glGetString ( GL_SHADING_LANGUAGE_VERSION );

	if ( glGetError() != GL_NO_ERROR )
		return "1.051";

	return std::string ( slVer );
}

int     Program :: maxVertexUniformComponents ()
{
	GLint maxVertexUniformComponents;

	glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents );

	return maxVertexUniformComponents;
}

int     Program :: maxVertexAttribs ()
{
	GLint maxVertexAttribs;

	glGetIntegerv ( GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs );

	return maxVertexAttribs;
}

int     Program :: maxFragmentTextureUnits ()
{
	GLint maxFragmentTextureUnits;

	glGetIntegerv ( GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentTextureUnits );

	return maxFragmentTextureUnits;
}

int     Program :: maxVertexTextureUnits ()
{
	GLint maxVertexTextureUnits;

	glGetIntegerv ( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureUnits );

	return maxVertexTextureUnits;
}

int     Program :: maxCombinedTextureUnits ()
{
	GLint maxCombinedTextureUnits;

	glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureUnits );

	return maxCombinedTextureUnits;
}

int     Program :: maxVaryingFloats ()
{
	GLint maxVaryingFloats;

	glGetIntegerv ( GL_MAX_VARYING_FLOATS, &maxVaryingFloats );

	return maxVaryingFloats;
}

int     Program :: maxFragmentUniformComponents ()
{
	GLint maxFragmentUniformComponents;

	glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents );

	return maxFragmentUniformComponents;
}

int	Program :: maxGeometryOutputVertices ()
{
	int	maxVerts;
	
	glGetIntegerv ( GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxVerts );

	return maxVerts;
}

int Program :: maxTessEvalOutputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, &m );
	
	return m;
}

int Program :: maxVertexOutputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_VERTEX_OUTPUT_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxGeometryUniformComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxGeometryInputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_GEOMETRY_INPUT_COMPONENTS, &m );
	
	return m;
}

int Program :: maxGeometryTotalOutputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxGeometryOutputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &m );
	
	return m;
}

int Program :: maxTessControlUniformComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS, &m );
	
	return m;
}

int Program :: maxCombinedTessControlUnfiromComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxTessControlInputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxTessControlOutputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxTessPatchComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_PATCH_COMPONENTS, &m );
	
	return m;
}

int Program :: maxTessControlTotalOutputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxTessGenLevel ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_GEN_LEVEL, &m );
	
	return m;
}

int	Program :: maxTessEvalUniformComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, &m );
	
	return m;
}

int Program :: maxCombinedTessEvalUniformComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS, &m );
	
	return m;
}

int Program :: maxTessEvalInputComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, &m );
	
	return m;
}

int		Program :: maxTransformFeedbackSeparateAttribs ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &m );
	
	return m;
}

int		Program :: maxTransformFeedbackInterleavedComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &m );
	
	return m;
}

int	Program :: maxUniformBufferBindings ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_UNIFORM_BUFFER_BINDINGS, &m );
	
	return m;
}
/*
int	Program :: maxCombinedUniformComponents ()
{
	int	m;
	
	glGetIntegerv ( GL_MAX_COMBINED_UNIFORM_COMPONENTS, &m );
	
	return m;
}
*/

bool	Program :: isSeparateShadersSupported ()
{
	return glewGetExtension ( "GL_ARB_separate_shader_objects" ) == GL_TRUE;
}
