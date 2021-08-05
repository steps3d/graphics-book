import math
import glm
from OpenGL.GL import *
import Window
import Program
import Mesh

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( -7, 0, 0 )
        self.light    = glm.vec3 ( -1, 1, 1 )
        self.shader   = Program.Program ( vertex = "cook-torrance.vsh", fragment = "cook-torrance.fsh" )
        self.mesh     = Mesh.Mesh.createKnot ( 1, 1, 120, 30 )
        self.shader.use ()

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv",       self.getRotation () )
        self.shader.setUniformMat ( "nm",       self.normalMatrix ( self.getRotation () ) )
        self.mesh.render()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )
        self.shader.setUniformVec ( "eye",   self.eye )
        self.shader.setUniformVec ( "light", self.light )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

    def idle ( self ):
        angle = 4 * self.time ()
        self.light = glm.vec3 ( 8*math.cos(angle), 8*math.sin(1.4*angle), 8+0.5*math.sin (angle/3) )
        self.shader.setUniformVec ( "eye",   self.eye   )
        self.shader.setUniformVec ( "light", self.light )

def main():
    win = MyWindow ( 900, 900, "Cook-Torrance shading model" )
    win.run ()

if __name__ == "__main__":
    main()
