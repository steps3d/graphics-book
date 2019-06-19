template <class Shape>
class BvhNode
{
protected:
	bbox	box;		// bounding box
	bool	leaf;		// is it a leaf
	union
	{
		std::vector<BvhNode *> children;
		std::vector<Shape *>   shapes;
	} data;

public:
};
