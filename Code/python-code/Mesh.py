import math
import numpy
import glm
from OpenGL.GL import *

class    Mesh:
    def __init__ ( self ):
        self.vertices = []
        self.indices  = []
        self.vao      = 0
        self.vbo      = 0
        self.ibo      = 0
        self.stride   = 14 * 4      # 14 floats per vertex

    def addVertex ( self, pos, tex, normal, tangent = None, binormal = None ):
        self.vertices.append ( pos.x )
        self.vertices.append ( pos.y )
        self.vertices.append ( pos.z )
        self.vertices.append ( tex.x )
        self.vertices.append ( tex.y )
        self.vertices.append ( normal.x )
        self.vertices.append ( normal.y )
        self.vertices.append ( normal.z )
        
        if tangent is None:
            self.vertices.append ( 0 )
            self.vertices.append ( 0 )
            self.vertices.append ( 0 )
        else:
            self.vertices.append ( tangent.x )
            self.vertices.append ( tangent.y )
            self.vertices.append ( tangent.z )
       
        if binormal is None:
            self.vertices.append ( 0 )
            self.vertices.append ( 0 )
            self.vertices.append ( 0 )
        else:
            self.vertices.append ( binormal.x )
            self.vertices.append ( binormal.y )
            self.vertices.append ( binormal.z )

    def addFace ( self, v1, v2, v3 ):
        self.indices.append ( v1 )
        self.indices.append ( v2 )
        self.indices.append ( v3 )
        
    # return for vertex index tuple (pos, tex, n, t, b)
    def getVertex ( self, index ):
        i = index * 14
        return ( glm.vec3 ( self.vertices [i:i+3] ), glm.vec2 ( self.vertices [i+3:i+5] ), glm.vec3 ( self.vertices [i+5:i+8] ), glm.vec3 ( self.vertices [i+8:i+11] ), glm.vec3 ( self.vertices [i+11:i+14] ) )

    def create ( self ):
        self.vao = glGenVertexArrays ( 1 )
        glBindVertexArray ( self.vao )

        self.vbo = glGenBuffers ( 1 )
        glBindBuffer ( GL_ARRAY_BUFFER, self.vbo )
        glBufferData ( GL_ARRAY_BUFFER, (len(self.vertices))*4, numpy.array(self.vertices, dtype = numpy.float32), GL_STATIC_DRAW )

        self.ibo = glGenBuffers ( 1 )
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, self.ibo )
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, len(self.indices)*4, numpy.array(self.indices, dtype= numpy.uint32), GL_STATIC_DRAW )

        # need pos, tex, normal, t, b
        glVertexAttribPointer     ( 0, 3, GL_FLOAT, GL_FALSE, 56, ctypes.c_void_p(0))
        glEnableVertexAttribArray ( 0 )

        #texCoord
        glVertexAttribPointer     ( 1, 2, GL_FLOAT, GL_FALSE, 56, ctypes.c_void_p(12))
        glEnableVertexAttribArray ( 1 )

        # normal
        glVertexAttribPointer ( 2, 3, GL_FLOAT, GL_FALSE, 56, ctypes.c_void_p(20))
        glEnableVertexAttribArray(2)

        # binormal
        glVertexAttribPointer ( 3, 3, GL_FLOAT, GL_FALSE, 56, ctypes.c_void_p(32))
        glEnableVertexAttribArray(3)

        # tangent
        glVertexAttribPointer ( 4, 3, GL_FLOAT, GL_FALSE, 56, ctypes.c_void_p(44))
        glEnableVertexAttribArray(4)
        #print ( 'num vertices=', len(self.vertices)/14, 'num faces = ', len(self.indices)/3 )

    def render ( self ):
        glBindVertexArray ( self.vao )
        glDrawElements( GL_TRIANGLES, len(self.indices), GL_UNSIGNED_INT, None )

    @classmethod
    def createBox ( cls, pos, size, mat = None, invertNormal = False ):
        x2          = pos.x + size.x
        y2          = pos.y + size.y
        z2          = pos.z + size.z
        ns          = -1.0 if invertNormal else 1.0
        numVertices = 4*6            # 4 vertices per each face
        numTris     = 6*2           # 2 tris per face
        mesh        = Mesh ()
             
                                    # front face
        mesh.addVertex ( pos,                           glm.vec2 ( 0, 0 ),             glm.vec3 ( 0, 0, ns ), glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 1, 0 ) )
        mesh.addVertex ( glm.vec3 ( x2, pos.y, z2 ),    glm.vec2 ( size.x, 0 ),        glm.vec3 ( 0, 0, ns ), glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 1, 0 ) )
        mesh.addVertex ( glm.vec3 ( x2, y2, z2 ),       glm.vec2 ( size.x, size.y ),   glm.vec3 ( 0, 0, ns ), glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 1, 0 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, y2, z2 ),    glm.vec2 ( 0, size.y ),        glm.vec3 ( 0, 0, ns ), glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 1, 0 ) )

                                    # back face
        mesh.addVertex ( glm.vec3 ( x2, pos.y, pos.z ), glm.vec2 ( size.x, 0 ),     glm.vec3 ( 0, 0, -ns ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, -1, 0 ) )
        mesh.addVertex ( pos,                           glm.vec2 ( 0, 0 ),          glm.vec3 ( 0, 0, -ns ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, -1, 0 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, y2, pos.z ), glm.vec2 ( 0, size.x ),     glm.vec3 ( 0, 0, -ns ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, -1, 0 ) )
        mesh.addVertex ( glm.vec3 ( x2, y2, pos.z ),    size,                       glm.vec3 ( 0, 0, -ns ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, -1, 0 ) )

                                    # left face
        mesh.addVertex ( pos,                           glm.vec2 ( 0, 0 ),           glm.vec3 ( -ns, 0, 0 ), glm.vec3 ( 0, 0, -1 ), glm.vec3 ( 0, -1, 0 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, pos.y, z2 ), glm.vec2 ( 0, size.z ),      glm.vec3 ( -ns, 0, 0 ), glm.vec3 ( 0, 0, -1 ), glm.vec3 ( 0, -1, 0 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, y2, z2 ),    glm.vec2 ( size.y, size.z ), glm.vec3 ( -ns, 0, 0 ), glm.vec3 ( 0, 0, -1 ), glm.vec3 ( 0, -1, 0 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, y2, pos.z ), glm.vec2 ( size.y, 0 ),      glm.vec3 ( -ns, 0, 0 ), glm.vec3 ( 0, 0, -1 ), glm.vec3 ( 0, -1, 0 ) )

                                    # right face
        mesh.addVertex ( glm.vec3 ( x2, pos.y, z2 ),     glm.vec2 ( 0, size.z ),      glm.vec3 ( ns, 0, 0 ), glm.vec3 ( 0, 0, 1 ),  glm.vec3 ( 0, 1, 0 ) )
        mesh.addVertex ( glm.vec3 ( x2, pos.y, pos.z ),  glm.vec2 ( 0, 0 ),           glm.vec3 ( ns, 0, 0 ), glm.vec3 ( 0, 0, 1 ),  glm.vec3 ( 0, 1, 0 ) )
        mesh.addVertex ( glm.vec3 ( x2, y2, pos.z ),     glm.vec2 ( size.y, 0 ),      glm.vec3 ( ns, 0, 0 ), glm.vec3 ( 0, 0, 1 ),  glm.vec3 ( 0, 1, 0 ) )
        mesh.addVertex ( glm.vec3 ( x2, y2, z2 ),        glm.vec2 ( size.y, size.z ), glm.vec3 ( ns, 0, 0 ), glm.vec3 ( 0, 0, 1 ),  glm.vec3 ( 0, 1, 0 ) )

                                    # top face
        mesh.addVertex ( glm.vec3 ( pos.x, y2, z2 ),     glm.vec2 ( 0, size.z ),      glm.vec3 ( 0, ns, 0 ),  glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 0, 1 ) )
        mesh.addVertex ( glm.vec3 ( x2, y2, z2 ),        glm.vec2 ( size.x, size.z ), glm.vec3 ( 0, ns, 0 ),  glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 0, 1 ) )
        mesh.addVertex ( glm.vec3 ( x2, y2, pos.z ),     glm.vec2 ( size.x, 0 ),      glm.vec3 ( 0, ns, 0 ),  glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 0, 1 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, y2, pos.z ),  glm.vec2 ( 0, 0 ),           glm.vec3 ( 0, ns, 0 ),  glm.vec3 ( 1, 0, 0 ), glm.vec3 ( 0, 0, 1 ) )

                                    # bottom face
        mesh.addVertex ( glm.vec3 ( x2, pos.y, z2 ),       glm.vec2 ( size.x, size.z ), glm.vec3 ( 0, -ns, 0 ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, 0, -1 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, pos.y, z2 ),    glm.vec2 ( 0, size.z ),      glm.vec3 ( 0, -ns, 0 ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, 0, -1 ) )
        mesh.addVertex ( glm.vec3 ( pos.x, pos.y, pos.z ), glm.vec2 ( 0, 0 ),           glm.vec3 ( 0, -ns, 0 ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, 0, -1 ) )
        mesh.addVertex ( glm.vec3 ( x2, pos.y, pos.z ),    glm.vec2 ( size.x, 0 ),      glm.vec3 ( 0, -ns, 0 ), glm.vec3 ( -1, 0, 0 ), glm.vec3 ( 0, 0, -1 ) )

        for face in range ( 6 ):
            mesh.addFace ( face * 4,     face * 4 + 1, face * 4 + 2 )
            mesh.addFace ( face * 4 + 2, face * 4 + 3, face * 4     )

        if mat != None:
            pass
               
        mesh.create ()
        
        return mesh

    @classmethod
    def createKnot ( cls, r1, r2, rings, sides ):
        def knot1D ( t ):
            r   = 1.8 + 0.8 * math.cos ( 3*t )
            phi = 0.2 * math.pi * math.sin ( 3*t )
            return r * glm.vec3 ( math.cos ( phi ) * math.sin ( 2*t ), math.cos ( phi ) * math.cos ( 2*t ), math.sin ( phi ) )
            
        def knot ( u, v ):
            t = glm.normalize ( knot1D ( u + 0.01 ) - knot1D ( u - 0.01 ) )
            b = glm.normalize ( glm.cross ( t, glm.vec3 ( 0, 0, 1 ) ) )
            n = glm.cross ( t, b )
            
            n = math.sin ( v ) * b + math.cos ( v ) * n
            b = glm.cross ( n, t )
            
            return ( knot1D ( u ) + 0.6 * n, n, t, b )
        
        ringDelta   = 2.0 * math.pi / rings
        sideDelta   = 2.0 * math.pi / sides
        invRings    = 1.0 / rings
        invSides    = 1.0 / sides
        numVertices = (sides+1)*(rings+1)
        numTris     = sides * rings * 2
        mesh        = Mesh ()
        
        for i in range ( rings + 1 ):
            phi = i * ringDelta
            for j in range ( sides + 1 ):
                psi = j * sideDelta
                
                pos, n, t, b = knot ( phi, psi )
                
                mesh.addVertex ( pos, glm.vec2 ( j * invSides, i * invRings ), n, t, b )
                
        for i in range ( rings  ):
            for j in range ( sides ):
                i1 = i + 1
                j1 = j + 1
                mesh.addFace ( i*(sides+1) + j, i1*(sides+1) + j,  i1*(sides+1) + j1 )
                mesh.addFace ( i*(sides+1) + j, i1*(sides+1) + j1, i*(sides+1) + j1 )
        
        mesh.create ()
        
        return mesh
