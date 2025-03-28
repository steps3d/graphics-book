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

#define GLM_FORCE_SWIZZLE

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
	glm::vec4	diffColor;

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

	const std::string&	getSpecMap () const
	{
		return specMap;
	}
	
	void setBumpMap ( const std::string& n )
	{
		bumpMap = n;
	}

	const std::string& getBumpMap () const
	{
		return bumpMap;
	}
	
	void	setDiffColor ( const glm::vec4& c )
	{
		diffColor = c;
	}
	
	const glm::vec4&	getDiffColor () const
	{
		return diffColor;
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

	VertexArray&	getVao ()
	{
		return vao;
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

class	MeshNode
{
	std::string				name;
	glm::mat4 				transform;
	MeshNode			  * parent = nullptr;
	std::vector<MeshNode *>	children;
	BasicMesh             * mesh   = nullptr;
	
public:
	MeshNode ( const std::string& nodeName ) : name ( nodeName ) {}
	~MeshNode ()
	{
		
	}
	
	const std::string&	getName () const
	{
		return name;
	}
	
	const glm::mat4&	getTransform () const
	{
		return transform;
	}
	
	void	setTransform ( const glm::mat4& m )
	{
		transform = m;
	}
	
	void	addChild ( MeshNode * node )
	{
		children.push_back ( node );
		node->parent = this;
	}

	const std::vector<MeshNode *>& getChildren () const
	{
		return children;
	}
	
	void	setParent ( MeshNode * p )
	{
		parent = p;
	}
	
	MeshNode * getParent ()
	{
		return parent;
	}
	
	const MeshNode * getParent () const
	{
		return parent;
	}
	
	void	render ( const glm::mat4& mv ) const
	{
		glm::mat4	mt = mv * transform;
		
		//setUniformMat ( mt );		XXX
		// set material
		
		if ( mesh )
			mesh -> render ();
		
		for ( auto it : children )
			it->render ( mt ); 
	}
	
};

class	CompositeMesh
{
	std::vector<MeshNode      *>	nodes;
	std::vector<BasicMesh     *>	meshes;
	std::vector<BasicMaterial *>	matInfos;
	
public:
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
BasicMesh * createKnot    ( const glm::vec3& org, float r1, float r2, int rings, int sides );
BasicMesh * loadMesh      ( const char * fileName, float scale = 1.0f );
bool 		loadAllMeshes ( const char * fileName, MultiMesh& mesh, std::vector<BasicMaterial *>& materials, float scale  = 1.0f );


#endif
