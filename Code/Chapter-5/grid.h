#ifndef	__GRID__
#define	__GRID__

#include	<list>
#include	<unordered_set>
#include	"bbox.h"
#include	"ray.h"

template<class Shape>
class RegularGrid
{
public:
	struct Node
	{
		std::list<Shape *> shapes;
	};
	
protected:
	int	n1, n2, n3;		// grid size
	bbox	box;			// bounding box for all shapes
	float	h1, h2, h3;		// steps for all axes
	Node  * nodes;			// array of n1*n2*n3 nodes
	
	RegularGrid  ( int n1, int n2, int n3, const std::list<Shape *>& shapes );
	~RegularGrid ()
	{
		delete [] nodes;
	}
	
	Node * getNode ( int i, int j, int k ) const
	{
		return (i*n1 + j)*n2 + k;
	}
	
	bbox getNodeBox ( int i, int j, int k ) const
	{
		glm::vec3 start ( box.getMinPoint () + glm::vec3 ( i*h1, j*h2, k*h3 ) );
		
		return bbox ( start, start + glm::vec3 ( h1, h2, h3 ) );
	}
	
	void	nodeForPoint ( const glm::vec3& p, int& i, int&j , int&k ) const
	{
		i = (int)((p.x - box.getMinPoint ().x) / h1);
		j = (int)((p.y - box.getMinPoint ().y) / h2);
		k = (int)((p.z - box.getMinPoint ().z) / h3);
	}
					// build bounding bxo for a list of shapes
	static bbox  buildBox ( const std::list<Shape *>& polys );
	
					// build a set of all shapes intersecting given area
	std::unordered+set<Shape *> findShapes ( const bbox& area ) const;
	
					// locate closes intersection with a ray
	Shape * findIntersection ( const ray& r , float& t ) const;
};

template<class Shape>
RegularGrid::RegularGrid ( int n1, int n2, int n3, const std::list<Shape *>& shapes )
{
	box   = buildBox ( shapes );
	h1    = box.getSize ().x / n1;
	h2    = box.getSize ().y / n2;
	h3    = box.getSize ().z / n3;
	nodes = new Node [n1*n2*n3];
	
	for ( auto it = shapes.begin (); it != shapes.end (); ++it )
	{
		bbox	b ( (*it)->getBounds () );
		int		i1, j1, k1, i2, j2, k2;
		
		nodeForPoint ( b.getMinPoint (), i1, j1, k1 );
		nodeForPoint ( b.getMaxPoint (), i2, j2, k3 );
		
		for ( int i = i1; i <= i2; i++ )
			for ( int j = j1; j <= j2; j++ )
				for ( int k = k1; k <= k2; k++ )
					getNode ( i, j, k )->shapes.push_front ( *it );
	}
}

template<class Shape>
bbox  regularGrid::buildBox ( const std::list<Shape *>& lst ) const
{
	bbox	b;

	for ( auto it = lst.begin (); it != lst.end (); ++it )
		b.merge ( (*it) -> getBounds () );

	return b;
}

std::unordered_set<Shape *> RegularGrid::findShapes ( const bbox& area ) const
{
	std::unordered_set<Shape *> result;
	int							i1, j1, k1, i2, j2, k2;
	
	nodeForPoint ( area.getMinPoint (), i1, j1, k1 );
	nodeForPoint ( area.getMaxPoint (), i2, j2, k3 );
	
	for ( int i = i1; i <= i2; i++ )
		for ( int j = j1; j <= j2; j++ )
			for ( int k = k1; k <= k2; k++ )
				for ( auto it : getNode ( i, j, k )->shapes )
					if ( area.intersects ( (*it)->getBounds () ) )
						result.insert ( *it );
				
	return result;
}

#endif
