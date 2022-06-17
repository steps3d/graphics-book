import math
import glm
from OpenGL.GL import *
import Window
import Program
import loadObj

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( -7, 0, 0 )
        self.lightDir = glm.vec3 ( -1, 1, 1 )
        self.kd       = 0.8
        self.ka       = 0.2
        self.ks       = 0.2
        self.length   = 1
        self.mesh     = loadObj.loadObj ( '../../Models/dragon.obj', scale = 20 )
        self.fur      = Program.Program ( glsl = "hedgehog.glsl" )
        self.shader   = Program.Program ( glsl = "blinn-phong.glsl" )
        self.center = (self.mesh.min + self.mesh.max) * 0.5
        self.transf = glm.translate ( glm.mat4(1), -self.center )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.use ()
        self.shader.setUniformMat ( "mv", self.getRotation () * self.transf )
        self.shader.setUniformMat ( "nm", self.normalMatrix ( self.getRotation () ) )
        self.mesh.render()

        self.fur.use ()
        self.fur.setUniformMat ( "mv", self.getRotation () * self.transf )
        self.fur.setUniformMat ( "nm", self.normalMatrix ( self.getRotation () ) )
        self.fur.setUniformMat ( "proj",  self.getProjection () )
        self.fur.setUniformInt ( "length", self.length )
        self.mesh.render()


    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.use ()
        self.shader.setUniformMat   ( "proj",  self.getProjection () )
        self.shader.setUniformVec   ( "eye",   self.eye )
        self.shader.setUniformVec   ( "lightDir", self.lightDir )
        self.shader.setUniformFloat ( "kd", self.kd )
        self.shader.setUniformFloat ( "ks", self.ks )
        self.shader.setUniformFloat ( "ka", self.ka )

    def key ( self, key, scancode, action, mods ):
        print ( key, action )
        super().key ( key, scancode, action, mods )
        if key == 61 and action == 1:
            self.length += 1
        elif key == 45 and action == 1 and self.length > 0:
            self.length -= 1
  

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

def main():
    win = MyWindow ( 900, 900, "Anisotropic shading model" )
    win.run ()

if __name__ == "__main__":
    main()
