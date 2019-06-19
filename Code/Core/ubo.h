//
// UBO wrapper
//

#include	<GL/glew.h>

template<typename T>
class Ubo
{
	GLuint	id      = 0;
	T     * ptr     = nullptr;
	int		binding = -1;

public:
	Ubo  () = default;
	~Ubo ()
	{
		if ( id != 0 )
		{
			glUnmapNamedBuffer ( id );
			glDeleteBuffers    ( 1, &id );
		}
	}

	bool	isOk () const
	{
		return (id != 0) && (ptr != nullptr);
	}
	
	bool	create ()
	{
		GLuint mapFlags     = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT |
							  GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
		GLuing storageFlags = GL_DYNAMIC_STORAGE_BIT | mapFlags;

		glCreateBuffers      ( 1, &id );
		glNamedBufferStorage ( id, sizeof ( T ), nullptr, storageFlags );

		ptr = (T *)glMapNamedBufferRange ( id, 0, sizeof ( T ), mapFlags );
		
		return ptr != nullptr;
	}

	void bind ( int index = 0 )
	{
		glBindBufferBase ( GL_UNIFORM_BUFFER_OBJECT, binding = index, id );	
	}
	
	T * operator -> () 
	{
		return ptr;
	}
};
