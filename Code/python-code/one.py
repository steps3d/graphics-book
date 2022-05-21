import numpy
from OpenGL.GL import *
import Window
import Program

vertices = [
	-1.0, -1.0, 0.0,
	 0.0,  1.0, 0.0,
	 1.0, -1.0, 0.0
]

class   MyWindow ( Window.Window ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t, fullScreen = False )

        self.vao = glGenVertexArrays(1)
        self.vbo = glGenBuffers(1)
        glBindVertexArray ( self.vao )
        glBindBuffer      ( GL_ARRAY_BUFFER, self.vbo )
        glBufferData      ( GL_ARRAY_BUFFER, 4 * len(vertices), numpy.array ( vertices, dtype = numpy.float32), GL_STATIC_DRAW )
                                                   # position attribute (loc=0, size=3, float, do not normalize, stride, offset)
        glVertexAttribPointer     ( 0, 3, GL_FLOAT, GL_FALSE, 4*3, ctypes.c_void_p(0) )
        glEnableVertexAttribArray ( 0 )

        self.shader   = Program.Program ( vertex = "simple.vsh", fragment = "simple.fsh" )
        self.shader.use ()

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )
        glDrawArrays ( GL_TRIANGLES, 0, 3 )

def main():
    win = MyWindow ( 900, 900, "Render triangle" )
    win.run ()

if __name__ == "__main__":
    main()
