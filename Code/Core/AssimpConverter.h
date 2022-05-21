#pragma once

#include	<glm/gtc/type_ptr.hpp>			// for make_mat
#include	<glm/glm.hpp>
#include	<glm/gtc/matrix_transform.hpp>
#include	<assimp/Importer.hpp> 
#include	<assimp/scene.h>     
#include	<assimp/postprocess.h>
#include	<assimp/cimport.h>
#include	<assimp/DefaultLogger.hpp>

				// helper functions for Assimp converting
inline std::string	toString ( const aiString& s )
{
	return std::string ( s.C_Str () );
}

inline glm::vec3 toGlmVec3 ( const aiVector3D &v )
{ 
	return glm::vec3(v.x, v.y, v.z); 
}

inline glm::vec2 toGlmVec2 ( const aiVector2D &v )
{ 
	return glm::vec2(v.x, v.y); 
}

inline glm::quat toGlmQuat ( const aiQuaternion& q )
{ 
	return glm::quat ( q.w, q.x, q.y, q.z ); 
}

inline glm::mat4 toGlmMat4 ( const aiMatrix4x4 &m )
{ 
	return glm::transpose(glm::make_mat4(&m.a1)); 
}

inline glm::mat3 toGlmMat3 ( const aiMatrix3x3 &m ) 
{ 
	return glm::transpose(glm::make_mat3(&m.a1)); 
}

