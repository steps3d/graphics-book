//
#ifndef	__SCEENE__
#define	__SCENE__

#include	"BasicMesh.h"
#include	"Program.h"
#include	"Texture.h"
#include	<vector>

class	Scene
{
	struct	Binding
	{
		int	  binding;
		Texture * texture;

		Binding ( int b, Texture * tex ) : binding ( b ), texture ( tex ) {}
	};

	struct	Group
	{
		Program                * program;
		std::vector<Binding>	 bindings;
		std::vector<BasicMesh *> meshes;

		Group ( Program * p ) : program ( p ) {}

		Group& addBinding ( int b, Texture * tex )
		{
			bindings.push_back ( Binding ( b, tex ) );

			return *this;
		}

		Group& addMesh ( BasicMesh * mesh )
		{
			meshes.push_back ( mesh );

			return *this;
		}

		void	render ();
	};

	std::vector<Group *> groups;
public:
	Scene () {}
	~Scene ()
	{

	}

	Group& addGroup ( Program * p )
	{
		Group * group = new Group ( p );
		
		groups.push_back ( group );

		return *group;
	}
	
	void render ()
	{
		for ( auto it : groups )
			it->render ();
	}
};

#endif
