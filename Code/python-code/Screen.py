import numpy
from OpenGL.GL import *

class    Screen:
    def __init__ ( self ):
        self.vertices = []
        self.vao      = 0
        self.vbo      = 0
        self.stride   = 14 * 4      # 14 floats per vertex
        self.addVertex ( -1,  1, 0, 1 )
        self.addVertex (  1,  1, 1, 1 )
        self.addVertex ( -1, -1, 0, 0 )
        self.addVertex (  1, -1, 1, 0 )
        self.create    ()

    def addVertex ( self, x, y, s, t ):
        self.vertices.append ( x )
        self.vertices.append ( y )
        self.vertices.append ( s )
        self.vertices.append ( t )

    def create ( self ):
        self.vao = glGenVertexArrays ( 1 )
        glBindVertexArray ( self.vao )

        self.vbo = glGenBuffers ( 1 )
        glBindBuffer ( GL_ARRAY_BUFFER, self.vbo )
        glBufferData ( GL_ARRAY_BUFFER, (len(self.vertices))*4, numpy.array(self.vertices, dtype = numpy.float32), GL_STATIC_DRAW )

        glVertexAttribPointer     ( 0, 4, GL_FLOAT, GL_FALSE, 4*4, ctypes.c_void_p(0))
        glEnableVertexAttribArray ( 0 )

    def render ( self ):
        glBindVertexArray ( self.vao )
        glDrawArrays      ( GL_TRIANGLE_STRIP, 0, 4 )
