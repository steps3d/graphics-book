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
        self.texture = Texture.Texture ( "uv.jpg" )
        #self.texture = Texture.Texture ( '../../Textures/brick.tga' )
        self.flow    = Texture.Texture ( "flowmap-2.png" )
        self.mesh    = Screen.Screen ()
        self.shader  = Program.Program ( glsl = "flow-5.glsl" )
        self.shader.use         ()
        self.shader.setTexture  ( "image",   0 )
        self.shader.setTexture  ( "flowMap", 1 )
        self.texture.bind ( 0 )
        self.flow.bind    ( 1 )

    def redisplay ( self ):
        glClearColor ( 0.5, 0.5, 0.5, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )

        self.shader.use         ()
        self.shader.setUniformFloat ( "time", self.time () )
        self.mesh.render ()

def main():
    win = MyWindow ( 900, 900, "Flow texturing" )
    win.run ()

if __name__ == "__main__":
    main()
