//
// Simple ray class
//

#include	<glm/geometric.hpp>
#include	"ray.h"

ray :: ray ( const glm::vec3& theOrigin, const glm::vec3& theDir, bool normalizeDir )
{
	org = theOrigin;
	dir = theDir;
	
	if ( normalizeDir )
		dir = glm::normalize ( dir );
}

