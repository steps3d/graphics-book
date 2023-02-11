import math
import glm
from OpenGL.GL import *
import Window
import Program
import Texture
import loadObj
import Mesh

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( -7, 0, 0 )
        self.lightDir = glm.vec3 ( -1, 1, 1 )
        self.kd       = 0.8
        self.ka       = 0.2
        self.ks       = 0.2
        self.texture  = Texture.Texture ( '../../Textures/Bumpmaps/normal2.bmp'  )
        #self.mesh     = loadObj.loadObj ( '../../Models/dragon.obj', scale = 20 )
        #self.mesh     = loadObj.loadObj ( '../../Models/buddha.obj', scale = 2 )
        #self.mesh     = loadObj.loadObj ( '../../Models/teapot.obj', scale = 0.1 )
        #self.mesh     = loadObj.loadObj ( '../../Models/bunny.obj', scale = 2 )
        #self.mesh     = loadObj.loadObj ( '../../Models/armadillo.obj', scale = 2 )
        self.mesh     = loadObj.loadObj ( '../../Models/duck.obj', scale = 3 )
        #self.mesh       = Mesh.Mesh.createSphere ( glm.vec3 ( 0 ), 3, 100, 50 )
        #self.mesh       = Mesh.Mesh.createSphere ( glm.vec3 ( 0 ), 2, 100, 50 )

        self.center = (self.mesh.min + self.mesh.max) * 0.5
        self.transf = glm.translate ( glm.mat4(1), -self.center )
        self.texture.bind       ( 0 )
        self.shader   = Program.Program ( glsl = "triplanar-normal-whiteout.glsl" )
        self.shader.use         ()
        self.shader.setTexture  ( "image", 0 )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv", self.getRotation () * self.transf )
        self.shader.setUniformMat ( "nm", self.normalMatrix ( self.getRotation () ) )
        self.mesh.render()

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
    win = MyWindow ( 900, 900, "Triplanar normal mapping Whiteout" )
    win.run ()

if __name__ == "__main__":
    main()
