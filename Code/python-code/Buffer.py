"""
    Wrapper for OpenGL VBO.
"""

from OpenGL.GL import *
import numpy
#import OpenGL.GL.shaders
#import glm

class    Buffer:
    def __init__ ( self, target, data ):
        self.target = target
        self.id     = glGenBuffers ( 1 )

        glBindBuffer ( self.target, self.id )
        glBufferData ( self.target, 4*len(data), numpy.array(data, dtype = numpy.float32), GL_STATIC_DRAW )
        print(self.id, len(data))
        assert self.id > 0, "Invalid buffer"

    def bind ( self, target = None ):
        if target is None:
            target = self.target
        glBindBuffer ( target, self.id )

    def unbind ( self, target = None ):
        if target is None:
            target = self.target
        glBindBuffer ( target, 0 )

    def bindBase ( self, target, index ):
        self.target = target
        glBindBufferBase ( target, index, self.id )

    def bindRange ( self, target, index, offset, size ):
        self.target = target
        glBindBufferRange ( target, index, id, offset, size )
