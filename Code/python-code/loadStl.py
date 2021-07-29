import numpy
import stl
import glm
import mesh

def _getVec3 ( v ):
    return glm.vec3 ( v [0], v [1], v [2] )

def loadStl ( filename ):
    _mesh = stl.mesh.Mesh.from_file(filename)
    if _mesh is None:
        return None
        
    m = mesh.Mesh ()
    for i in range ( len ( _mesh.points ) ):    # iterate through faces
        m.addVertex ( _getVec3 ( _mesh.points [i][0:3] ), glm.vec2 ( 0, 0 ), _getVec3 ( _mesh.normals [i] ) )
        m.addVertex ( _getVec3 ( _mesh.points [i][3:6] ), glm.vec2 ( 0, 0 ), _getVec3 ( _mesh.normals [i] ) )
        m.addVertex ( _getVec3 ( _mesh.points [i][6:9] ), glm.vec2 ( 0, 0 ), _getVec3 ( _mesh.normals [i] ) )
        m.addFace   ( i*3, i*3+1, i*3+2 )
        
    m.create ()
    return m
