import math
import glm
import numpy
from OpenGL.GL import *
import Window
import Program
import Texture
import Mesh
import Framebuffer
import dds

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.fb        = Framebuffer.Framebuffer ( 512, 512, depth = True )
        self.fb.bind          ()
        self.fb.attachTexture ( self.fb.createTexture () )
        self.fb.unbind        ()
        self.mesh    = Mesh.Mesh.createKnot ( 1, 4, 120, 30 )
        self.cube    = Mesh.Mesh.createBox ( glm.vec3 ( -1, -1, -1 ), glm.vec3 ( 2, 2, 2 ) )
        self.texture = Texture.Texture ( "../../Textures/Fieldstone.dds" )
        self.shader  = Program.Program ( glsl = "rotate-8.glsl" )
        self.shader.use         ()
        self.shader.setTexture  ( "image", 0 )
        self.eye = glm.vec3  ( 2.5, 2.5, 2.5 )

    def redisplay ( self ):
        self.displayBoxes ()

        glClearColor ( 0.5, 0.5, 0.5, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.use         ()
        self.fb.colorBuffers [0].bind ()
        self.shader.setUniformMat ( "mv",  self.getRotation () )
        self.shader.setUniformMat ( "nm",  self.normalMatrix ( self.getRotation () ) )
        self.shader.setUniformVec ( "eye", self.eye )
        self.cube.render ()
        self.fb.colorBuffers [0].unbind ()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

    def displayBoxes ( self ):
        self.fb.bind ()
        self.texture.bind ()
        self.shader.setUniformMat ( "mv",  glm.rotate(glm.mat4(1), self.time (), glm.vec3(0, 0, 1)))
        self.shader.setUniformVec ( "eye", glm.vec3 ( 7, 7, 7 ) )

        glClearColor ( 0, 0, 0, 1 )
        glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

        self.mesh.render    ()
        self.texture.unbind ()
        self.shader.unbind  ()
        self.fb.unbind      ()

def main():
    win = MyWindow ( 900, 900, "Environmapped knot" )
    win.run ()

if __name__ == "__main__":
    main()

