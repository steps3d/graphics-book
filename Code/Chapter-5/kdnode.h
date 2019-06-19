#ifndef	__KD_NODE__
#define	__KD_NODE__

#include	<list>
#include	<unordered_set>
#include	"bbox.h"
#include	"plane.h"
#include	"ray.h"

template <class Shape>
class kDNode
{
protected:
	bbox	           box;		// boduning box for this node
	plane	 	   	   p;		// splitting plane
	kDNode           * pos;		// positive node
	kDNode           * neg;		// negative node
	bool			   isLeaf;
	std::list<Shape *> shapes;	// shapes (for leafs only)
public:

	kDNode ( const bbox& b ) : box ( b )
	{
		pos    = nullptr;
		neg    = nullptr;
		isLeaf = true;
	}
	
	kDNode ( const std::list<Shape *>& lst, const bbox& b ) : shapes ( lst ), box ( b )
	{
		pos    = nullptr;
		neg    = nullptr;
		isLeaf = true;
	}
	
	static kDNode * buildKdTree ( const std::list<Shape *> sh );

private:
					// choose splitting plane for box
	static plane splitBox    ( const bbox& b ) const;
					// create lists of pos/neg shapes
	static void  splitShapes ( const std::list<Shape *>& src, std::list<Shape *>& p, std::list<Shape *>& n );

					// build AABB for list of shapes
	static bbox  buildBox ( const std::list<Shape *>& lst );

					// return all shapes intersecting given box
	void findShapes ( const bbox& area, std::unordered_set<Shape *>& result ) const;
	
					// return closest intersection with a given ray
	Shape * findIntersection ( const ray& r ) const
	{
		
	}
};

template <class Shape>
plane kDNode::splitBox ( const bbox& b ) const
{
	glm::vec3	n ( 1, 0, 0 );
	glm::vec3	size   = b.getSize ();
	glm::vec3	center = b.getCenter ();
	int			iMax   = 0;
	float		vMax   = size.x;

	if ( size.y > vMax )
	{
		iMax = 1;
		vMax = size.y;
	}

	if ( size.z > vMax )
	{
		iMax = 2;
		vMax = size.z;
	}

	if ( iMax == 1 )
		n = glm::vec3 ( 0, 1, 0 );
	else
	if ( iMax == 2 )
		n = glm::vec3 ( 0, 0, 1 );

	float	d = -glm::dot ( center, n );

	return plane ( n, d );
}

template<class Shape>
bbox  kDNode::buildBox ( const std::list<Shape *>& lst ) const
{
	bbox	b;

	for ( auto it = lst.begin (); it != lst.end (); ++it )
		b.merge ( (*it) -> getBounds () );

	return b;
}

template<class Shape>
void kDNode::splitShapes ( const std::list<Shape *>& src, std::list<Shape *>& p std::list<Shape *>& n )
{
	for ( auto it = src.begin (); it != src.end (); ++it )
	{
		switch ( p.classify ( (*it)->getBounds () )
		{
			case IN_POSITIVE:
				p.push_front ( *it );
				break;

			case IN_NEGATIVE:
				n.push_front ( *it );
				break;

			default:
				p.push_front ( *it );
				n.push_front ( *it );
		}
	}
}

template <class Shape>
kDNode * kDNode::buildKdTree ( const std::list<Shape *> lst )
{
	bbox b ( buildBox ( lst ) );

	if ( lst.size () < MAX_CHILDS )
		return new kDNode ( lst, b );

	plane              p ( splitBox ( b) );
	std::list<Shape *> ps;
	std::list<Shape *> ng;

	splitShapes ( lst, ps, ng );
	
	if ( ps.empty () || ng.empty () )
		return new kDNode ( lst, b );

	kDNode * root = new kDNode ( b );

	root->p      = p;
	root->pos    = buildKdTree ( ps );
	root->neg    = buildKdTree ( ng );
	root->isLeaf = false;
	
	return root;
}

void kDNode::findShapes ( const bbox& area, std::unordered_set<Shape *>& result ) const
{
	if ( !box.intersects ( area ) )
		return;

	if ( pos != nullptr )
		pos->findShapes ( area, result );

	if ( neg != nullptr )
		neg->findShapes ( area, result );

	for ( auto it = shapes.begin (); it != shapes.end (); ++it )
		if ( area.intersects ( (*it) ->getBounds () ) )
			result.insert ( *it );
}

Shape * kDNode::findIntersection ( const ray& r ) const
{
			// no intersection
	if ( !box.intersects ( area ) )
		return nullptr;

	Shape * ptr = nullptr;
	
			// find which child is closer to ray
	if ( glm::dot ( p.n, r.getDir () ) >= 0 )
	{		// 1st pos, 2nd - neg
		if ( pos != nullptr )
			ptr = pos->findIntersection ( r );

		if ( ptr != nullptr )
			return ptr;
		
		if ( neg != nullptr )
			ptr = neg->findIntersection ( r );
		
		if ( ptr != nullptr )
			return ptr;
	}
	else	// 1st neg, 2nd pos
	{
		if ( neg != nullptr )
			ptr = neg->findIntersection ( r );
		
		if ( ptr != nullptr )
			return ptr;
		
		if ( pos != nullptr )
			ptr = pos->findIntersection ( r );

		if ( ptr != nullptr )
			return ptr;
	}
	
	float	tMin = FLT_MAX;
	float	t;
	
	for ( auto it = shapes.begin (); it != shapes.end (); ++it )
		if ( (*it) ->getBounds ().intersects ( r ) )
			if ( (*it)->findIntersection ( r, t ) )
				if ( t < tMin )
				{
					tMin = t;
					ptr  = *it;
				}
				
	return ptr;
}

#endif
