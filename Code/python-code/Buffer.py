"""
    Wrapper for OpenGL VBO.
"""

from OpenGL.GL import *
import OpenGL.GL.shaders
import glm

class    Buffer:
    def __init__ ( self, target, data ):
        self.target = target
        self.id     = glGenBuffers ( 1 )

        glBindBuffer ( self.target, self.id )
        glBufferData ( self.target, len(data), numpy.array(data, dtype = numpy.uint8), GL_STATIC_DRAW )
        assert self.id > 0, "Invalid buffer"

    def bind ( self, target = None ):
        if target is None:
            target = self.type
        glBindBuffer ( self.type, self.id )

    def bindBase ( self, target, index ):
        self.target = target
        glBindBufferBase ( self.target, index, self.id )

    def bindRange ( self, target, index, offset, size ):
        self.target = target
        glBindBufferRange ( self.target, index, id, offset, size )
