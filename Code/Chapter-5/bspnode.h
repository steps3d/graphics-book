#ifndef	__BSP_NODE__
#define	__BSP_NODE__

#include	"bbox.h"
#include	"plane.h"
#include	"ray.h"

class BspNode
{
	bbox	  box;		// boduning box for subtree
	plane	  p;		// splitting plane for this node
	polygon * poly;		// polygon used for building the plane
	BspNode * pos;
	BspNode * neg;
public:
	BspNode ( polygon * p, const bbox& b ) : box ( b ), poly ( p ) 
	{
		p   = p -> getPlane ();
		pos = nullptr;
		neg = nullptr;
	}

	static BspNode * buildTree    ( const std::list<polygon *>& polys );
	static polygon * bestSplitter ( cost std::list<polygon *>& polys );
	static void	    splitPolys    ( const plane& p,
	                                const std::list<polygon *>& polys, 
                                    std::list<polygon *>& positive,
                                    std::list<polygon *>& negative );
	static bbox  buildBox         ( const std::list<polygon *>& polys );
	
					// perform back-to-front traversal of the tree
	template <class Observer>
	void traverse ( const glm::vec3& observer, Visitor& visitor )
	{
		if ( p.classify ( observer ) == IN_POSITIVE )
		{
			if ( neg != nullptr )
				neg->traverse ( observer, visitor );
			visitor.process ( poly );
			if ( pos != nullptr )
				pos->traverse ( observer, visitor );
		}
		else
		{
			if ( pos != nullptr )
				pos->traverse ( observer, visitor );
			visitor.process ( poly );
			if ( neg != nullptr )
				neg->traverse ( observer, visitor );
		}
	}
};

BspNode * BspNode::buildTree ( const std::list<polygon *>& polys )
{
	polygon * splitter = bestSplitter ( polys );
	bbox	  box      = buildBox ( polys );
	BspNode * root     = new BspNode ( splitter, box );
	
				// build positive and negative lists
	std::list<polygon *> pos, neg;
	
	splitPolys ( root-> p, polys, pos, neg );
	
	if ( pos.empty () || neg.empty () )
		return root;
	
	root->pos = buildTree ( pos );
	root->neg = buildTree ( neg );
	
	return root;
}

void BspNBode::splitPolys ( const plane& p, const std::list<polygon *>& polys,
                            std::list<polygon>& pos, std::list<polygon *>& neg )
{
	polygon * p1, * p2;
	
	for ( auto it : polys )
	{
		switch ( p.classify ( *it ) )
		{
			case IN_POSITIVE:
				pos.push_front ( *it );
				break;
				
			case IN_NEGATIVE:
				neg.push_front ( *it );
				break;
				
			case IN_PLANE:
				pos.push_front ( *it );
				break;
				
			default:		// IN_BOTH
				(*it)->split ( p, p1, p2 );
				pos.push_front ( p1 );
				neg.push_front ( p2 );
		}
	}
}

template<class Shape>
bbox  BspNode::buildBox ( const std::list<Shape *>& lst ) const
{
	bbox	b;

	for ( auto it : lst )
		b.merge ( (*it) -> getBounds () );

	return b;
}

#endif
