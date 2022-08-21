"""
    Wrapper for VAO - specifications of vertex attributes and bindings,
    wrapped in VAO object
    
    Example:
        fmt = VertexFormat().add ( vbuf, id = 0, dim = 3 ).add ( vbuf, id = 1, dim = 2 ).addIndices ( ibuf ).create ()
        fmt.bind ()
        glDrawElements ()
        fmt.unbind ()
"""

from OpenGL.GL import *

class   VertexFormat:
    def __init__ ( self, sz = 0 ):
        self.size  = sz
        self.vao   = 0
        self.index = None
        self.attrs = []
        
    def add ( self, buf, id, dim, type = GL_FLOAT, normalized = False, offs = -1 ):
        self.attrs.append ( { 'buffer': buf, 'id': id, 'offs': offs, 'type': type, 'dim': dim, 'normalized': normalized } )
        return self

    def addIndices ( self, buf ):
        self.index = buf
        return self

    def create ( self, reserveSpace = 0 ):
        self.vao  = glGenVertexArrays ( 1 )
        self.size = self._totalSize () + reserveSpace
        offset    = 0

        glBindVertexArray ( self.vao )

        if self.index:
            self.index.bind ()

        for attr in self.attrs:
            buf        = attr ['buffer']
            id         = attr ['id']
            offs       = attr ['offs']
            normalized = GL_TRUE if attr ['normalized'] else GL_FALSE

            if offs == -1:
                offs = offset

            buf.bind ()

            glVertexAttribPointer     ( id, attr ['dim'], attr ['type'], normalized, self.size, ctypes.c_void_p(offs) )
            glEnableVertexAttribArray ( id )

            offset += self._attrSize ( attr ['dim'], attr ['type'] )

        glBindVertexArray ( 0 )

        return self

    def bind ( self ):
        glBindVertexArray ( self.vao )

    def unbind ( self ):
        glBindVertexArray ( 0 )

    def _attrSize ( self, dim, type ):
        unitSize = 4        # GL_FLOAT, GL_INT, GL_UNSIGNED_INT
        if type == GL_BYTE or type == GL_UNSIGNED_BYTE:
            unitSize = 1
        elif type == GL_SHORT or type == GL_UNSIGNED_SHORT: # or type == GL_HALF:
            unitSize = 2

        return dim * unitSize

    def _totalSize ( self ):
        sz = 0
        for attr in self.attrs:
            sz += self._attrSize ( attr ['dim'], attr ['type'] )

        return sz
