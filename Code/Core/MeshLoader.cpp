#include "BasicMesh.h"
#include "AssimpConverter.h"
//#include <assimp/pbrmaterial.h>

/*
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/DefaultLogger.hpp>
*/

//#ifdef _WIN32
//#pragma comment(lib, "assimp.lib") 
//#endif

static void loadAiMesh ( const aiMesh * mesh, float scale, std::vector<BasicVertex>& vertices, std::vector<int>& indices, int base = 0 )
{
	const aiVector3D	zero3D (0.0f, 0.0f, 0.0f);
		
	for ( size_t i = 0; i < mesh->mNumVertices; i++ ) 
	{
		aiVector3D&  pos    = mesh->mVertices[i];
		aiVector3D&  normal = mesh->mNormals[i];
		aiVector3D   tex;
		aiVector3D   tangent, binormal;
		BasicVertex  v;
			
		if (mesh->HasTextureCoords(0))
			tex = mesh->mTextureCoords[0][i];
		else
			tex = zero3D;
			
		tangent  = (mesh->HasTangentsAndBitangents()) ? mesh->mTangents[i]   : zero3D;
		binormal = (mesh->HasTangentsAndBitangents()) ? mesh->mBitangents[i] : zero3D;

		v.pos = scale * glm::vec3 ( pos.x, pos.y, pos.z );
		v.tex = glm::vec2 ( tex.x, 1.0f - tex.y );
		v.n   = glm::vec3 ( normal.x, normal.y, normal.z );
		v.t   = glm::vec3 ( tangent.x, tangent.y, tangent.z );
		v.b   = glm::vec3 ( binormal.x, binormal.y, binormal.z );

		vertices.push_back ( v );
	}

	for ( size_t i = 0; i < mesh->mNumFaces; i++ ) 
	{
		const aiFace& face = mesh->mFaces[i];
			
		assert(face.mNumIndices == 3);
		
		indices.push_back( base + face.mIndices[0] );
		indices.push_back( base + face.mIndices[1] );
		indices.push_back( base + face.mIndices[2] );
	}
}
		
static BasicMesh * loadMeshFromMesh ( const aiMesh * mesh, float scale )
{
	std::vector<BasicVertex> vertices;
	std::vector<int>         indices;
	
	loadAiMesh ( mesh, scale, vertices, indices );
	
	auto * myMesh = new BasicMesh ( vertices.data (), indices.data (), vertices.size (), indices.size () / 3 /*mesh->mNumFaces*/ );

	myMesh->setName ( mesh->mName.C_Str() );

	return myMesh;
}

BasicMesh * loadMesh ( const char * fileName, float scale )
{
	Assimp::Importer importer;
	const int        flags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
	const aiScene  * scene = importer.ReadFile ( fileName, flags );

	if ( scene == nullptr )
		return nullptr;
		
	return 	loadMeshFromMesh ( scene -> mMeshes [0], scale );
}

bool loadMeshAndMaterials ( const char * fileName, std::vector<BasicMesh*>& meshes, std::vector<BasicMaterial *>& materials, float scale )
{
	Assimp::Importer importer;
	
	importer.SetPropertyBool ( AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, true );
	
	Assimp::DefaultLogger::create( "assimp.log",  Assimp::Logger::VERBOSE, aiDefaultLogStream_FILE);

	// aiProcess_PreTransformVertices causes animations to be dropped !!!
	// aiProcess_FlipWindingOrder - OK
	// aiProcess_Triangulate  - OK
	// aiProcess_PreTransformVertices - SKIP ANIMATIONS
	// aiProcess_CalcTangentSpace - OK
	// aiProcess_GenSmoothNormals - OK
	//const int        flags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcessPreset_TargetRealtime_MaxQuality;
	const int        flags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure / aiProcess_JoinIdenticalVertices;// | aiProcessPreset_TargetRealtime_MaxQuality;

	//printf ( "Validate %d\n", importer.ValidateFlags ( flags ) );

	const aiScene  * scene = importer.ReadFile ( fileName, flags );
	std::string		 path ( fileName );
	auto			 p1     = path.rfind ( '/' );
	auto			 p2     = path.rfind ( '\\' );

	if (scene == nullptr)
		return false;

	printf ( "mNumAnimations = %d\n", scene->mNumAnimations);

	if ( p1 == std::string::npos )
		p1 = p2;
	else
	if ( p2 != std::string::npos )
		p1 = std::min ( p1, p2 );

	if ( p1 != std::string::npos )
		path = path.substr ( 0, p1 ) + "/";
	else
		path = "";
	
	printf ( "Path %s\n", path.c_str () );
	
	if ( scene == nullptr )
		return false;
		
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial*  material = scene->mMaterials[i];
		aiString		   name;
		BasicMaterial	 * mat = new BasicMaterial;

		material->Get(AI_MATKEY_NAME, name);
		
		mat -> setName ( name.C_Str());
		
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &name, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
			{
				printf ( "\tDiffuse %s\n", name.C_Str () );
				mat->setDiffuseMap ( path + name.C_Str() );
			}

		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &name, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
			{
				printf ( "\tSpecular %s\n", name.C_Str () );
				mat->setSpecMap ( path + name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
			if (material->GetTexture(aiTextureType_NORMALS, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tNormals %s\n", name.C_Str () );
				mat->setBumpMap ( path + name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
			if (material->GetTexture(aiTextureType_SHININESS, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tShininess %s\n", name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
			if (material->GetTexture(aiTextureType_HEIGHT, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tHeight %s\n", name.C_Str () );
				mat->setBumpMap ( path + name.C_Str () );
			}

		materials.push_back ( mat );
	}

	auto * root = scene->mRootNode;

		// root->mNumChildren, root->mChildren keep hierarchy organization of model
		// we link with meshes through names (mName)
		// root->mName.C_Str ()
		// root->mTransform - aiMatrix4x4
		// root->mParent
		// root->mChildren[], root->mNumChildren

	for ( int i = 0; i < scene->mNumMeshes; i++ )
	{
		if ( scene->mMeshes [i]->mNormals == nullptr )		// sometimes happen
			continue;

		auto        matIndex = scene->mMeshes[i]->mMaterialIndex;
		BasicMesh * mesh     = loadMeshFromMesh ( scene -> mMeshes [i], scale );
		
		mesh -> setMaterial ( matIndex );
		meshes.push_back ( mesh );
	}

	//delete scene;

	return true;
}

bool loadAllMeshes ( const char * fileName, MultiMesh& mesh, std::vector<BasicMaterial *>& materials, float scale )
{
	Assimp::Importer importer;
	const int        flags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
	const aiScene  * scene = importer.ReadFile ( fileName, flags );
	std::string		 path ( fileName );
	auto			 p1     = path.rfind ( '/' );
	auto			 p2     = path.rfind ( '\\' );

	if ( p1 == std::string::npos )
		p1 = p2;
	else
	if ( p2 != std::string::npos )
		p1 = std::min ( p1, p2 );

	if ( p1 != std::string::npos )
		path = path.substr ( 0, p1 ) + "/";
	else
		path = "";
	
	printf ( "Path %s\n", path.c_str () );
	
	if ( scene == nullptr )
		return false;
		
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial*  material = scene->mMaterials[i];
		aiString		   name;
		BasicMaterial	 * mat = new BasicMaterial;

		material->Get(AI_MATKEY_NAME, name);
		
		mat -> setName ( name.C_Str());
		
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tDiffuse %s\n", name.C_Str () );
				mat->setDiffuseMap ( path + name.C_Str() );
			}

		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tSpecular %s\n", name.C_Str () );
				mat->setSpecMap ( path + name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
			if (material->GetTexture(aiTextureType_NORMALS, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tNormals %s\n", name.C_Str () );
				mat->setBumpMap ( path + name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
			if (material->GetTexture(aiTextureType_SHININESS, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tShininess %s\n", name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
			if (material->GetTexture(aiTextureType_HEIGHT, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tHeight %s\n", name.C_Str () );
				mat->setBumpMap ( path + name.C_Str () );
			}

		//material->GetTexture("$mat.gltf.pbrMetallicRoughness.baseColorFactor", 0, &name, NULL, NULL, NULL, NULL, NULL);
		//printf ( "PBR: %s\n", name.C_Str() );
		
		materials.push_back ( mat );
	}

	size_t	vc = 0;		// vertex count
	size_t	ic = 0;		// index count
	
	for ( int i = 0; i < scene->mNumMeshes; i++ )
	{
						// append vertices and indices to global index
		loadAiMesh ( scene->mMeshes [i], scale, mesh.vertices, mesh.indices, mesh.vertices.size () );
		
		mesh.counts.push_back      ( mesh.indices.size () - ic );
		mesh.materials.push_back   ( scene->mMeshes[i]->mMaterialIndex );
		
		ic = mesh.indices.size  ();
		vc = mesh.vertices.size ();
	}

	mesh.numMeshes = scene->mNumMeshes;
	ic             = 0;
	
						// create a list of pointers to indices for each mesh
	for ( int i = 0; i < mesh.numMeshes; i++ )
	{
		mesh.indicesList.push_back ( /*mesh.indices.data () +*/(int *) (ic*sizeof(int)) );
		ic += mesh.counts [i];
	}

	computeNormals ( mesh.vertices.data (), mesh.indices.data (), vc,  ic / 3 );

	mesh.box.reset ();
	
	for ( auto& v : mesh.vertices )
		mesh.box.addVertex ( v.pos );

	//delete scene;
	
	return true;
}

/*
MatInfo ( aiMaterial* mat )
{
	aiString property;    //contains filename of texture
	
	if ( AI_SUCCESS != mat->Get(AI_MATKEY_NAME, property))
	{
			std::cerr << "Material without a name is not handled." << std::endl;
			std::cerr << "Model " << this->name << std::endl;
			exit(-1);
			//return nullptr; should work too.
	}
	
	name = toString ( property );
	
	if ( mat->GetTextureCount(aiTextureType_AMBIENT) > 0 ) 
	{
		if (AI_SUCCESS == mat->GetTexture(aiTextureType_AMBIENT, 0, &property)) {
			if(property.data[0] != '*') 
			{
				//newMaterial->setAmbientTexture(property.C_Str());
				props["ambient"] = toString ( property );
				std::cout << "set ambient texture " << property.C_Str() << std::endl;
			} else 
			{
				//embeddedTexture handling
				//newMaterial->setAmbientTexture(property.C_Str(), &this->name);
				std::cout << "set (embedded) ambient texture " << property.C_Str() << "|" << this->name<< std::endl;
			}
		} 
		else 
			std::cerr << "The model contained ambient texture information, but texture loading failed. \n" <<
					  "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
	}
	
	if ( mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 ) 
	{
		if (AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, 0, &property)) 
		{
			if(property.data[0] != '*') 
			{
				//newMaterial->setDiffuseTexture(property.C_Str());
				props["diffuse"] = toString ( property );					
			} 
			else 
			{
				//embeddedTexture handling
				newMaterial->setDiffuseTexture(property.C_Str(), &this->name);
			}
		} 
		else 
			std::cerr << "The model contained diffuse texture information, but texture loading failed. \n" <<
					  "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
	}
	
	if ( mat->GetTextureCount(aiTextureType_SPECULAR) > 0) 
	{
		if (AI_SUCCESS == mat->GetTexture(aiTextureType_SPECULAR, 0, &property))
		{
			if(property.data[0] != '*') 
			{
				newMaterial->setSpecularTexture(property.C_Str());
				std::cout << "set specular texture " << property.C_Str() << std::endl;
			} 
			else 
			{
				//embeddedTexture handling
				newMaterial->setSpecularTexture(property.C_Str(), &this->name);
				std::cout << "set (embedded) setSpecularTexture texture " << property.C_Str() << "|" << this->name<< std::endl;
			}
		} 
		else 
			std::cerr << "The model contained specular texture information, but texture loading failed. \n" <<
					  "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
	}
	
	if ( mat->GetTextureCount(aiTextureType_NORMALS) > 0) 
	{
		if (AI_SUCCESS == mat->GetTexture(aiTextureType_NORMALS, 0, &property))
		{
			if(property.data[0] != '*') 
			{
				newMaterial->setNormalTexture(property.C_Str());
				std::cout << "set normal texture " << property.C_Str() << std::endl;
			} 
			else 
			{
				//embeddedTexture handling
				newMaterial->setNormalTexture(property.C_Str(), &this->name);
				std::cout << "set (embedded) setNormalTexture texture " << property.C_Str() << "|" << this->name<< std::endl;
			}
		} 
		else
			std::cerr << "The model contained normal texture information, but texture loading failed. \n" <<
					  "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
	}

#define AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR "$mat.gltf.pbrMetallicRoughness.baseColorFactor", 0, 0
#define AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR "$mat.gltf.pbrMetallicRoughness.metallicFactor", 0, 0
#define AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR "$mat.gltf.pbrMetallicRoughness.roughnessFactor", 0, 0
#define AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE aiTextureType_DIFFUSE, 1
#define AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE aiTextureType_UNKNOWN, 0
#define AI_MATKEY_GLTF_ALPHAMODE "$mat.gltf.alphaMode", 0, 0
#define AI_MATKEY_GLTF_ALPHACUTOFF "$mat.gltf.alphaCutoff", 0, 0
#define AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS "$mat.gltf.pbrSpecularGlossiness", 0, 0
#define AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR "$mat.gltf.pbrMetallicRoughness.glossinessFactor", 0, 0
	
}
*/

/*		
 if (AI_SUCCESS == currentMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
            newMaterial->setAmbientColor(GLMConverter::AssimpToGLM(color));
        } else {
            newMaterial->setAmbientColor(glm::vec3(0,0,0));
        }

        if (AI_SUCCESS == currentMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
            newMaterial->setDiffuseColor(GLMConverter::AssimpToGLM(color));
        } else {
            newMaterial->setDiffuseColor(glm::vec3(0,0,0));
        }

        if (AI_SUCCESS == currentMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
            newMaterial->setSpecularColor(GLMConverter::AssimpToGLM(color));
        } else {
            newMaterial->setSpecularColor(glm::vec3(0,0,0));
        }

        if (AI_SUCCESS == currentMaterial->Get(AI_MATKEY_SHININESS, transferFloat)) {
            newMaterial->setSpecularExponent(transferFloat);
        } else {
            newMaterial->setSpecularExponent(0);
        }

        if ((currentMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)) {
            if (AI_SUCCESS == currentMaterial->GetTexture(aiTextureType_AMBIENT, 0, &property)) {
                if(property.data[0] != '*') {
                    newMaterial->setAmbientTexture(property.C_Str());
                    std::cout << "set ambient texture " << property.C_Str() << std::endl;
                } else {
                    //embeddedTexture handling
                    newMaterial->setAmbientTexture(property.C_Str(), &this->name);
                    std::cout << "set (embedded) ambient texture " << property.C_Str() << "|" << this->name<< std::endl;
                }

            } else {
                std::cerr << "The model contained ambient texture information, but texture loading failed. \n" <<
                          "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
            }
        }
        if ((currentMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)) {
            if (AI_SUCCESS == currentMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &property)) {
                if(property.data[0] != '*') {
                    newMaterial->setDiffuseTexture(property.C_Str());
                } else {
                    //embeddedTexture handling
                    newMaterial->setDiffuseTexture(property.C_Str(), &this->name);
                }
            } else {
                std::cerr << "The model contained diffuse texture information, but texture loading failed. \n" <<
                          "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
            }
        }

        if ((currentMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)) {
            if (AI_SUCCESS == currentMaterial->GetTexture(aiTextureType_SPECULAR, 0, &property)) {
                if(property.data[0] != '*') {
                    newMaterial->setSpecularTexture(property.C_Str());
                    std::cout << "set specular texture " << property.C_Str() << std::endl;
                } else {
                    //embeddedTexture handling
                    newMaterial->setSpecularTexture(property.C_Str(), &this->name);
                    std::cout << "set (embedded) setSpecularTexture texture " << property.C_Str() << "|" << this->name<< std::endl;
                }
            } else {
                std::cerr << "The model contained specular texture information, but texture loading failed. \n" <<
                          "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
            }
        }

        if ((currentMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)) {
            if (AI_SUCCESS == currentMaterial->GetTexture(aiTextureType_NORMALS, 0, &property)) {
                if(property.data[0] != '*') {
                    newMaterial->setNormalTexture(property.C_Str());
                    std::cout << "set normal texture " << property.C_Str() << std::endl;
                } else {
                    //embeddedTexture handling
                    newMaterial->setNormalTexture(property.C_Str(), &this->name);
                    std::cout << "set (embedded) setNormalTexture texture " << property.C_Str() << "|" << this->name<< std::endl;
                }
            } else {
                std::cerr << "The model contained normal texture information, but texture loading failed. \n" <<
                          "TextureAsset path: [" << property.C_Str() << "]" << std::endl;
            }
        }

 **********************/

#if 0
const aiScene * loadAllMeshes2 ( const char * fileName, MultiMesh& mesh, std::vector<BasicMaterial *>& materials, float scale )
{
	Assimp::Importer importer;
	const int        flags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
	const aiScene  * scene = importer.ReadFile ( fileName, flags );
	std::string		 path ( fileName );
	auto			 p1     = path.rfind ( '/' );
	auto			 p2     = path.rfind ( '\\' );

	if ( p1 == std::string::npos )
		p1 = p2;
	else
	if ( p2 != std::string::npos )
		p1 = std::min ( p1, p2 );

	if ( p1 != std::string::npos )
		path = path.substr ( 0, p1 ) + "/";
	else
		path = "";
	
	printf ( "Path %s\n", path.c_str () );
	
	if ( scene == nullptr )
		return nullptr;
		
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial*  material = scene->mMaterials[i];
		aiString		   name;
		BasicMaterial	 * mat = new BasicMaterial;

		material->Get(AI_MATKEY_NAME, name);
		
		mat -> setName ( name.C_Str());
		
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tDiffuse %s\n", name.C_Str () );
				mat->setDiffuseMap ( path + name.C_Str() );
			}

		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tSpecular %s\n", name.C_Str () );
				mat->setSpecMap ( path + name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
			if (material->GetTexture(aiTextureType_NORMALS, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tNormals %s\n", name.C_Str () );
				mat->setBumpMap ( path + name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
			if (material->GetTexture(aiTextureType_SHININESS, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tShininess %s\n", name.C_Str () );
			}

		if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
			if (material->GetTexture(aiTextureType_HEIGHT, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				printf ( "\tHeight %s\n", name.C_Str () );
				mat->setBumpMap ( path + name.C_Str () );
			}

		materials.push_back ( mat );
	}

	size_t	vc = 0;		// vertex count
	size_t	ic = 0;		// index count
	
	for ( int i = 0; i < scene->mNumMeshes; i++ )
	{
						// append vertices and indices to global index
		loadAiMesh ( scene->mMeshes [i], scale, mesh.vertices, mesh.indices, mesh.vertices.size () );
		
		mesh.counts.push_back      ( mesh.indices.size () - ic );
		mesh.materials.push_back   ( scene->mMeshes[i]->mMaterialIndex );
		
		ic = mesh.indices.size  ();
		vc = mesh.vertices.size ();
	}

	mesh.numMeshes = scene->mNumMeshes;
	ic             = 0;
	
						// create a list of pointers to indices for each mesh
	for ( int i = 0; i < mesh.numMeshes; i++ )
	{
		mesh.indicesList.push_back ( /*mesh.indices.data () +*/(int *) (ic*sizeof(int)) );
		ic += mesh.counts [i];
	}

	computeNormals ( mesh.vertices.data (), mesh.indices.data (), vc,  ic / 3 );

	mesh.box.reset ();
	
	for ( auto& v : mesh.vertices )
		mesh.box.addVertex ( v.pos );
	
	return scene;
}
#endif


inline MeshNode  * createNode ( aiNode * anode )
{
	MeshNode * node = new MeshNode ( toString ( anode-> mName ) );
	
	node -> setTransform ( toGlmMat4 ( anode->mTransformation ) );
	
	for ( int i = 0; i < anode->mNumChildren; i++ )
		node->addChild ( createNode ( anode->mChildren [i] ) );
	
	return node;
}

