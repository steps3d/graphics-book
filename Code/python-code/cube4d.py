import math
import numpy
import glm
from OpenGL.GL import *
import Window
import Program
import Mesh

    # append vertex of 4D unit cube with given index
def appendVertex ( data, index ):
    data.append ( 1.0 if index & 1 else 0.0 )
    data.append ( 1.0 if index & 2 else 0.0 )
    data.append ( 1.0 if index & 4 else 0.0 )
    data.append ( 1.0 if index & 8 else 0.0 )

    # build edges - pairs of vertices
    # edge index differs in only one bit
def prepareBuffers ():
    data = []
    for index in range ( 16 ):
        for j in range ( 4 ):
            if index & (1 << j):
                continue
        
            appendVertex ( data, index )
            appendVertex ( data, index | (1 << j) )
            
    return data
        
class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.angle1  = 0.0
        self.angle2  = 0.0
        self.step1   = 0.002
        self.step2   = 0.001
        self.e1      = glm.vec4 (  1,  2, -5,  9 )
        self.e2      = glm.vec4 ( -9,  4,  2,  7 )
        self.dir     = glm.vec4 (  1, -1,  3, -1 )
        self.data    = prepareBuffers ()      
        self.vao     = glGenVertexArrays ( 1 )
        self.vbo     = glGenBuffers      ( 1 )

        glDisable                 ( GL_DEPTH_TEST )
        glBindVertexArray         ( self.vao )
        glBindBuffer              ( GL_ARRAY_BUFFER, self.vbo )
        glBufferData              ( GL_ARRAY_BUFFER, (len(self.data))*4, numpy.array(self.data, dtype = numpy.float32), GL_STATIC_DRAW )
        glVertexAttribPointer     ( 0, 4, GL_FLOAT, GL_FALSE, 4*4, ctypes.c_void_p(0))
        glEnableVertexAttribArray ( 0 )

        self.shader  = Program.Program ( glsl = "4d.glsl" )
        self.shader.use     ()
        self.orthonormalize ()

    def redisplay ( self ):
        self.angle1 += self.step1
        self.angle2 += self.step2
        
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        
        self.drawCube ( self.angle1 - 80*self.step1, self.angle2 - 80*self.step2, glm.vec4 ( 0.2 ) )
        self.drawCube ( self.angle1 - 60*self.step1, self.angle2 - 60*self.step2, glm.vec4 ( 0.4 ) )
        self.drawCube ( self.angle1 - 40*self.step1, self.angle2 - 40*self.step2, glm.vec4 ( 0.6 ) )
        self.drawCube ( self.angle1 - 20*self.step1, self.angle2 - 20*self.step2, glm.vec4 ( 0.8 ) )
        self.drawCube ( self.angle1,                 self.angle2,                 glm.vec4 ( 1.0 ) )

    def drawCube ( self, a1, a2, color ):
        self.shader.setUniformFloat ( "sinA1", math.sin ( a1 ) )
        self.shader.setUniformFloat ( "cosA1", math.cos ( a1 ) )
        self.shader.setUniformFloat ( "sinA2", math.sin ( a2 ) )
        self.shader.setUniformFloat ( "cosA2", math.cos ( a2 ) )
        self.shader.setUniformVec   ( "color", color   )
        self.shader.setUniformVec   ( "e1",    self.e1 )
        self.shader.setUniformVec   ( "e2",    self.e2 )

        glBindVertexArray ( self.vao )
        glDrawArrays      ( GL_LINES, 0, len(self.data) // 4 )
    
    def    orthonormalize ( self ):
        dot      = glm.dot ( self.e1, self.e2 )
        len1Sq   = glm.dot ( self.e1, self.e1 )
        self.e2 -= self.e1 * dot / len1Sq
        self.e1  = glm.normalize ( self.e1 )
        self.e2  = glm.normalize ( self.e2 )

def main():
    win = MyWindow ( 900, 900, "4D Cube Animation" )
    win.run ()

if __name__ == "__main__":
    main()

