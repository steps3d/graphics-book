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
import Screen

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.mesh	= Screen.Screen ()
        self.shader  = Program.Program ( glsl = "organic.glsl" )
        self.shader.use     	()

    def redisplay ( self ):
        glClearColor ( 0.5, 0.5, 0.5, 1.0 )
        glClear  	( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )

        self.shader.use         	()
        self.shader.setUniformFloat ( "time", self.time () )
        self.mesh.render        	()

def main():
    win = MyWindow ( 900, 900, "Procedural organic" )
    win.run ()

if __name__ == "__main__":
    main()

