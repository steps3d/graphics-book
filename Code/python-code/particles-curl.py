from OpenGL.GL import *
import glm
import Window
import Program
import Buffer
import Texture
import dds
import numpy

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.vel  	= None
        self.pos  	= None
        self.eye  	= glm.vec3 ( 3, 3, 3 )
        self.compute  = Program.Program ( glsl = "particles-compute-curl.glsl" )
        self.render   = Program.Program ( glsl = "particles-render.glsl" )
        noise, target, width, height, depth	= dds.readDdsFile ( "../../Textures/noise-3D.dds" )
        glBindTexture ( GL_TEXTURE_3D, noise )
        self.compute.bind ()
        self.compute.setTexture ( "noiseMap", 0 )
        self.compute.unbind ()

        self.initParticles ( 32 )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.compute.bind ()
        glDispatchCompute ( self.numParticles // 64, 1, 1 )
        glMemoryBarrier   ( GL_SHADER_STORAGE_BARRIER_BIT )

        self.render.use ()
        self.render.setUniformMat ( "mv",   self.getRotation   () )
        self.render.setUniformMat ( "proj", self.getProjection () )
        glBindVertexArray ( self.vao )
        self.pos.bind     ( GL_ARRAY_BUFFER )
        glPointSize       ( 1.0 )
        glDrawArrays      ( GL_POINTS, 0, self.numParticles )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.5 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

    def initParticles ( self, num ):
        n                 = num
        self.numParticles = n * n * n

        	# init buffers with particle data
        vb = []
        pb = []
        h  = 2.0 / (n - 1)

        for i in range ( n ):
            for j in range ( n ):
                for k in range ( n ):
                    pb.append ( h * i - 1 )
                    pb.append ( h * j - 1 )
                    pb.append ( h * k - 1 )
                    pb.append ( 1 )

        	# create VBO'a and VAO
        self.pos = Buffer.Buffer ( GL_SHADER_STORAGE_BUFFER, numpy.array ( pb, dtype = numpy.float32 ) )
        self.pos.bindBase ( GL_SHADER_STORAGE_BUFFER, 0  )

        	# create and init VAO
        self.vao = glGenVertexArrays ( 1 )
        glBindVertexArray ( self.vao )
        self.pos.bind     ( GL_ARRAY_BUFFER )
        self.render.use   ()

        glVertexAttribPointer     ( 0, 4, GL_FLOAT, GL_FALSE, 16, ctypes.c_void_p(0) )
        glEnableVertexAttribArray ( 0 )

        self.render.unbind 	()
        glBindVertexArray  	( 0 )

        	# setup blending
        #glEnable    ( GL_BLEND )
        #glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA )

def main():
    win = MyWindow ( 900, 900, "Curl noise" )
    win.run ()

if __name__ == "__main__":
	main()

