from OpenGL.GL import *
import Window
import Program
import Texture
import Screen

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.radius   = 5
        self.mesh     = Screen.Screen ()
        self.image    = Texture.Texture ( 'hotel.jpg' )
        self.shader   = Program.Program ( glsl = "kuwahara.glsl" )
        self.shader.use ()
        self.image.bind ( 0 )
        self.shader.setTexture    ( "image", 0 )
        self.shader.setUniformInt ( "radius", self.radius )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.mesh.render()

def main():
    win = MyWindow ( 1536, 864, "Kuwahara image filter" )
    win.run ()

if __name__ == "__main__":
    main()
