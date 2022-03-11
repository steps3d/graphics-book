import numpy
from OpenGL.GL import *
import glm
import Window
import Program
import Font

class   MyWindow ( Window.Window ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.font = Font.Font ( 'Font/font.fnt', 'Font/font.png' )
        self.font.program.use        ()
        self.font.program.setTexture ( "fontMap",   0   )
        self.font.buildVertices      ( glm.vec2 ( 0, 0 ), 0.2   , "Test !" )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_BLEND )
        glBlendFunc  ( GL_ONE, GL_ONE )
        
        self.font.program.setUniformMat ( "mv",  glm.mat4 (1))
        self.font.render ()

def main():
    win = MyWindow ( 900, 900, "Render font" )
    win.run ()

if __name__ == "__main__":
    main()
