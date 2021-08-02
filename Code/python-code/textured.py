import math
import glm
import numpy
from OpenGL.GL import *
import Window
import Program
import Texture
import Mesh

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.shader    = Program.Program ( vertex = "textured.vsh", fragment = "textured.fsh" )
        self.mesh      = Mesh.Mesh.createKnot ( 1, 4, 120, 30 )
        self.texture   = Texture.Texture ( '../../Textures/block.jpg' ) 
		
        self.texture.bind       ( 0 )
        self.shader.use         ()
        self.shader.setTexture  ( "image", 0 )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv", self.getRotation () )
        self.mesh.render()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

def main():
    win = MyWindow ( 900, 900, "Textured example" )

    if not win:
        glfw.terminate()
        return

    win.run ()

if __name__ == "__main__":
    main()


