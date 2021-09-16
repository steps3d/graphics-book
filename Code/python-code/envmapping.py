import math
import glm
import numpy
from OpenGL.GL import *
import Window
import Program
import Texture
import Mesh
import dds

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.mesh      = Mesh.Mesh.createKnot ( 1, 4, 120, 30 )
        self.texture, target, w, h, d = dds.readDdsFile ( '../../Textures/Cubemaps/LobbyCube.dds' )
        self.shader    = Program.Program ( glsl = "envmapping.glsl" )
        glBindTexture ( GL_TEXTURE_CUBE_MAP, self.texture )
        self.shader.use         ()
        self.shader.setTexture  ( "image", 0 )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv",  self.getRotation () )
        self.shader.setUniformMat ( "nm",  self.normalMatrix ( self.getRotation () ) )
        self.shader.setUniformVec ( "eye", self.eye )
        self.mesh.render()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

def main():
    win = MyWindow ( 900, 900, "Environmapped knot" )
    win.run ()

if __name__ == "__main__":
    main()

