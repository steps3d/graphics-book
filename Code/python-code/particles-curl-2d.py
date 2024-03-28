from OpenGL.GL import *
import glm
import Window
import Program
import Buffer
import Texture
import numpy

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
    	super().__init__ ( w, h, t )
    	self.vel  	= None
    	self.pos  	= None
    	self.compute  = Program.Program ( glsl = "particles-compute-curl-2d.glsl" )
    	self.render   = Program.Program ( glsl = "particles-render-2d.glsl" )
    	self.noise	= Texture.Texture ( "../../Textures/noise-2D.png" )
    	self.noise.bind (1)
    	self.compute.bind ()
    	self.compute.setTexture ( "noiseMap", 1 )
    	self.compute.unbind ()

    	self.initParticles ( 32*4 )

    def redisplay ( self ):
    	glClearColor ( 0.2, 0.3, 0.2, 1.0 )
    	glClear  	( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
    	glEnable 	( GL_DEPTH_TEST )

    	self.compute.bind ()
    	self.compute.setUniformFloat ( "deltaT", 1.0 )
    	glDispatchCompute ( self.numParticles // 64, 1, 1 )
    	glMemoryBarrier   ( GL_SHADER_STORAGE_BARRIER_BIT )

    	self.render.use ()
    	#self.render.setUniformMat ( "mvp",  self.getProjection () * self.getRotation () )
    	glBindVertexArray ( self.vao )
    	self.pos.bind 	( GL_ARRAY_BUFFER )
    	glPointSize   	( 1.0 )
    	glDrawArrays  	( GL_POINTS, 0, self.numParticles )

    def initParticles ( self, num ):
    	n             	= num
    	self.numParticles = n * n
    	self.eye      	= glm.vec3 ( -0.5, 0.5, 25 )

        	# init buffers with particle data
    	vb = []
    	pb = []
    	h  = 2.0 / (n - 1)

    	for i in range ( n ):
        	for j in range ( n ):
                	pb.append ( h * i - 1 )
                	pb.append ( h * j - 1 )
                	pb.append ( 0 )
                	pb.append ( 1 )
                	vb.append ( 0 )
                	vb.append ( 0 )
                	vb.append ( 0 )
                	vb.append ( 0 )

        	# create VBO'a and VAO
    	self.vel = Buffer.Buffer ( GL_SHADER_STORAGE_BUFFER, numpy.array ( vb, dtype = numpy.float32 ) )
    	self.vel.bindBase ( GL_SHADER_STORAGE_BUFFER, 1  )
    	self.pos = Buffer.Buffer ( GL_SHADER_STORAGE_BUFFER, numpy.array ( pb, dtype = numpy.float32 ) )
    	self.pos.bindBase ( GL_SHADER_STORAGE_BUFFER, 0  )

        	# create and init VAO
    	self.vao = glGenVertexArrays ( 1 )
    	glBindVertexArray ( self.vao )
    	self.pos.bind      	( GL_ARRAY_BUFFER )
    	self.render.use    	()

    	glVertexAttribPointer 	( 0, 4, GL_FLOAT, GL_FALSE, 16, ctypes.c_void_p(0))
    	glEnableVertexAttribArray ( 0 )

    	#self.render.setAttrPtr ( "pos", 4, 4*4, ctypes.c_void_p(0) )
    	self.render.unbind 	()
    	glBindVertexArray  	( 0 )

        	# setup blending
    	#glEnable	( GL_BLEND )
    	#glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA )

def main():
	win = MyWindow ( 900, 900, "Compute particles" )
	win.run ()

if __name__ == "__main__":
	main()


