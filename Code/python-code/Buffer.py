"""
    Wrapper for OpenGL VBO.
"""

from OpenGL.GL import *
import numpy

class    Buffer:
    def __init__ ( self, target, data, dtype = numpy.float32 ):
        self.target = target
        self.id     = glGenBuffers ( 1 )

        assert self.id > 0, "Invalid buffer"
        glBindBuffer ( self.target, self.id )
        glBufferData ( self.target, 4*len(data), numpy.array(data, dtype = dtype), GL_STATIC_DRAW )

    '''
        Bind buffer to specific binding point
    '''
    def bind ( self, target = None ):
        if target is None:
            target = self.target
        glBindBuffer ( target, self.id )

    def unbind ( self, target = None ):
        if target is None:
            target = self.target
        glBindBuffer ( target, 0 )

    '''
        Bind buffer to (target, index)
    '''
    def bindBase ( self, target, index ):
        self.target = target
        glBindBufferBase ( target, index, self.id )

    '''
        Bind range of buffer to specific target, index
    '''
    def bindRange ( self, target, index, offset, size ):
        self.target = target
        glBindBufferRange ( target, index, id, offset, size )
    '''
        Change data in buffer to new data, reallocating GPU memory
    '''
    def setData ( self, data, dtype = numpy.float32 ):
        glBindBuffer ( self.target, self.id )
        #glBufferData ( self.target, 0, 0 )   # free buffer
        glBufferData ( self.target, 4*len(data), numpy.array(data, dtype = dtype), GL_STATIC_DRAW )
