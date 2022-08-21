import numpy
from OpenGL.GL import *
import Window
import Program
import Buffer
import VertexFormat

vertices = [
    -1.0, -1.0, 0.0,
     0.0,  1.0, 0.0,
     1.0, -1.0, 0.0
]

class   MyWindow ( Window.Window ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t, fullScreen = False )

        self.vbuf = Buffer.Buffer ( GL_ARRAY_BUFFER, numpy.array ( vertices, dtype = numpy.float32 ) )
        self.fmt  = VertexFormat.VertexFormat ().add ( self.vbuf, id = 0, dim = 3 ).create ().bind ()

        self.shader   = Program.Program ( vertex = "simple.vsh", fragment = "simple.fsh" )
        self.shader.use ()

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )
        glDrawArrays ( GL_TRIANGLES, 0, 6 )

def main():
    win = MyWindow ( 900, 900, "Render triangle" )
    win.run ()

if __name__ == "__main__":
    main()

