import glm
from OpenGL.GL import *
import Window
import Program
import Mesh
import Texture

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( -7, 0, 0 )
        self.lightDir = glm.vec3 ( -1, 1, 1 )
        self.kd       = 0.8
        self.ka       = 0.2
        self.ks       = 0.2
        self.texture  = Texture.Texture ( '../../Textures/block.jpg' ) 
        self.mesh     = Mesh.Mesh.createBox ( glm.vec3 ( -2 ), glm.vec3 ( 4 ) )
        self.shader   = Program.Program ( glsl = "tex-repeat-1.glsl" )
        self.shader.use         ()
        self.texture.bind       ( 0 )
        self.shader.setTexture  ( "image", 0 )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv",       self.getRotation () )
        self.shader.setUniformMat ( "nm",       self.normalMatrix ( self.getRotation () ) )
        self.mesh.render ()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat   ( "proj",  self.getProjection () )
        self.shader.setUniformVec   ( "eye",   self.eye )
        self.shader.setUniformVec   ( "lightDir", self.lightDir )
        self.shader.setUniformFloat ( "kd", self.kd )
        self.shader.setUniformFloat ( "ks", self.ks )
        self.shader.setUniformFloat ( "ka", self.ka )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

def main():
    win = MyWindow ( 900, 900, "Non-periodic texture repetition" )
    win.run ()

if __name__ == "__main__":
    main()
