import sys
from OpenGL.GL import *
import Window
import Program
#import Texture
import Screen
import glm

class   SDFWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t, shader ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( 0, 0, 3 )
        self.fov      = 1.4
        self.mesh     = Screen.Screen ()
        self.shader   = Program.Program ( glsl = shader if shader else "sdf-rounding.glsl" )
        self.shader.use ()
        self.shader.setUniformVec ( "eye", self.eye )

    def redisplay ( self ):
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glDisable    ( GL_DEPTH_TEST )

        self.shader.setUniformFloat ( "fov",  self.fov )
        self.shader.setUniformMat   ( "mv",   glm.mat3 ( self.getRotation () ) )
        self.shader.setUniformFloat ( "time", self.time () )
        self.mesh.render()
    def mouseScroll ( self, dx, dy ):
        self.fov += 0.05 * ( 1 if dy >= 0 else -1 )

def main():
    shader = sys.argv [1] if len ( sys.argv ) > 1 else None
    win = SDFWindow ( 900, 900, "SDF Rendering", shader = shader )
    win.run ()

if __name__ == "__main__":
    main()
