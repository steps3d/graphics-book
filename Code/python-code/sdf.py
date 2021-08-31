from OpenGL.GL import *
import Window
import Program
#import Texture
import Screen
import glm

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( 0, 0, 3 )
        self.mesh     = Screen.Screen ()
        self.shader   = Program.Program ( glsl = "sdf-3.glsl" )
        self.shader.use ()
        self.shader.setUniformVec ( "eye", self.eye )

    def redisplay ( self ):
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glDisable    ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv", glm.mat3 ( self.getRotation () ) )
        self.mesh.render()

def main():
    win = MyWindow ( 900, 900, "SDF Rendering" )
    win.run ()

if __name__ == "__main__":
    main()
