"""
    Wrapper for OpenGL VBO.
"""

from OpenGL.GL import *
import numpy

class    Buffer:
    def __init__ ( self, target, data, dtype = numpy.float32 ):
        #assert self.id > 0, "Invalid buffer"
        self.target = target
        self.id     = glGenBuffers ( 1 )

            # convert to numpy array for uploading
        arr = numpy.array ( data, dtype = dtype )
            # upload data to GPU
        glBindBuffer ( self.target, self.id )
        glBufferData ( self.target, arr.itemsize*arr.size, arr, GL_STATIC_DRAW )

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
