#ifndef	__R_TREE__
#define	__R_TREE__

#include	"bbox.h"

template <class Shape>
class RTreeNode
{
	struct Record
	{
		bbox	box;		// bounding box
		union 
		{
			RTreeNode * node;
			Shape     * shape;
		};
	};

	bbox	    box;	// AABB of the whole subtree
	int	    count;		// # of records in the node
	Record    * recs;	// records (childtrees)
	bool	    isLeaf;	// is this a leaf
	RTreeNode * parent;	// link to parent node
	
	void insertObject ( Shape * obj )
	{
		RTreeNode * node = leafForShape ( obj->getBounds () );
		Record    * rec  = new Record ( obj );
		
		node->insert ( rec );		
	}

	void insert ( Record * rec )
	{
		if ( count < M )
		{
			addRecord ( rec );
			box.merge ( rec -> box );
			updateParent ();
			
			return;
		}
		
		std::vector<Record>  r1, r2;
		
		split ( r1, r2 );
			
		if ( parent == nullptr )	// root
		{							// create two new nodes
			RTreeNode * newNode1 = new RTreeNode;
			RTreeNode * newNode2 = new RTreeNode;
				
			newNode1->setRecs ( r1 );
			newNode2->setRecs ( r2 );
			
			Record * rec1 = new Record ( newNode1 );
			Record * rec2 = new Record ( newNode2 );
			
									.// root contains {r1, r2}
			std::vector<Record> root = { rec1, rec2 };
			
			setRecs ( root );
		}
		else
		{
			RTreeNode * newNode1 = new RTreeNode;
			
			setRecs      ( r1 );
			addRecord    ( rec );
			updateParent ();
								// create new node
			newNode1->setRecs ( r2 );
			
			Record * rec1 = new Record ( newNode1 );
			
			parent->insert ( rec1 );
		}
	}
};

#endif
