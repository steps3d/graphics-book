"""
    Rendaerpass object
    Encapsulates rendering (possibly to frramebuffer object), clearing, 
    binding  required textures, 
    actual rendering ( object.render () or object () )
    and unbinding
"""

from OpenGL.GL import glClear, GL_COLOR_BUFFER_BIT

class	Renderpass:
    def __init__ ( self, clear = GL_COLOR_BUFFER_BIT, program = None, target = None, textures = None ):
        assert program is not None

        self.clear    = clear
        self.program  = program
        self.target   = target
        self.textures = textures if textures else []

    def render ( self, renderable ):
        if self.target is not None:
            self.target.bind ()

        if self.clear != 0:
            glClear ( self.clear )

        for tex, unit in  self.textures:
            tex.bind ( unit )

        with self.program:    
            if callable ( renderable ):     # it can be an object with render method or callable (e.g. lambda)
                renderable ()
            else:
                renderable.render  ()

        if self.target is not None:
            self.target.unbind ()
