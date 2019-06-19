//
// Class to hold simple meshes.
// For every vertex position, texture coordinate and TBN basis is stored
//

#pragma once
#ifndef	__BASIC_MESH__
#define	__BASIC_MESH__

#include <string>
#include <vector>

#define GLM_FORCE_RADIANS

#ifndef GLM_SWIZZLE
	#define GLM_SWIZZLE
#endif

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "bbox.h"

struct  BasicVertex
{
	glm::vec3	pos;
	glm::vec2	tex;
	glm::vec3	n;
	glm::vec3	t, b;

	BasicVertex () = default;
	BasicVertex ( const glm::vec3& p, const glm::vec2& t ) : pos ( p ), tex ( t ) {}
};

class	BasicMaterial 
{
	std::string	name;
	std::string	diffMap;
	std::string	specMap;
	std::string	bumpMap;
	Texture	    tex;

public:
	BasicMaterial  () = default;
	~BasicMaterial () = default;

	void setName ( const std::string& s )
	{
		name = s;
	}

	const std::string& getName () const
	{
		return name;
	}

	void setDiffuseMap ( const std::string& n )
	{
		diffMap = n;
	}

	const std::string& getDiffuseMap () const
	{
		return diffMap;
	}
	
	void setSpecMap ( const std::string& n )
	{
		specMap = n;
	}

	void setBumpMap ( const std::string& n )
	{
		bumpMap = n;
	}

	const std::string& getBumpMap () const
	{
		return bumpMap;
	}
	
	bool load ()
	{
		if ( diffMap.empty () )
			return true;
		
		return tex.load2D ( diffMap );
	}
	
	Texture& getTexture ()
	{
		return tex;
	}
};

class BasicMesh
{
	int	         numVertices;
	int	         numTriangles;
	VertexArray	 vao;			// array with all bindings
	VertexBuffer vertBuf;		// vertex data
	VertexBuffer indBuf;		// index buffer
	std::string  name;
	bbox		 box;
	int			 material;
	
public:
	BasicMesh ( BasicVertex * vertices, const int * indices, int nv, int nt );
	
	void	render ();
	void	renderInstanced ( int primCount );
	
	const std::string& getName () const
	{
		return name;
	}
	
	const bbox&	getBox () const
	{
		return box;
	}

	int getNumVertices () const
	{
		return numVertices;
	}
	
	int getNumTriangles () const
	{
		return numTriangles;
	}
	
	void setName ( const std::string& theName )
	{
		name = theName;
	}
	
	void setMaterial ( int m )
	{
		material = m;
	}
	
	int getMaterial () const
	{
		return material;
	}
};

struct MultiMesh
{
	std::vector<BasicVertex>	vertices;		// all vertices for all meshes
	std::vector<int>			indices;		// indices for all meshes
	std::vector<GLsizei>		counts;			// number of triangles for every mesh
	std::vector<int>			materials;		// material index for every mesh
	std::vector<int *>			indicesList;	// array of pointers to indices list (for glMultiDrawElements)
	int							numMeshes;
	bbox						box;
};


extern const float pi;

void	computeTangents ( BasicVertex& v0, const BasicVertex& v1, const BasicVertex& v2 );
void	computeNormals  ( BasicVertex * vertices, const int * indices, int nv, int nt );

BasicMesh * createSphere  ( const glm::vec3& org, float radius, int n1, int n2 );
BasicMesh * createQuad    ( const glm::vec3& org, const glm::vec3& dir1, const glm::vec3& dir2 );
BasicMesh * createHorQuad ( const glm::vec3& org, float s1, float s2 );
BasicMesh * createBox     ( const glm::vec3& pos, const glm::vec3& size, const glm::mat4 * mat = nullptr, bool invertNormal = false );
BasicMesh * createTorus   ( float r1, float r2, int n1, int n2 );
BasicMesh * createKnot    ( float r1, float r2, int n1, int n2 );
BasicMesh * loadMesh      ( const char * fileName, float scale = 1.0f );
bool 		loadAllMeshes ( const char * fileName, MultiMesh& mesh, std::vector<BasicMaterial *>& materials, float scale  = 1.0f );


#endif
