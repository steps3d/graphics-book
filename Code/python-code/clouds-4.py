import sys
from OpenGL.GL import *
import Window
import Program
import Texture
import Screen
import glm

class   SDFWindow ( Window.RotationWindow ):
	def __init__ ( self, w, h, t ):
		super().__init__ ( w, h, t )
		self.eye   	= glm.vec3 ( 0, 0, 7 )
		self.fov   	= 1.4
		#self.noise 	= Texture.Texture ( "noise/noise32.png" )
		self.noise 	= Texture.Texture.as3D ( "noise/noiseErosionPacked.tga", 32, 32, 32, True )
		self.blueNoise = Texture.Texture ( "noise/blue-noise.bmp" )
		self.mesh  	= Screen.Screen ()
		self.shader	= Program.Program ( glsl = "clouds-5.glsl" )

		self.noise.bind 	( 0 )
		self.blueNoise.bind ( 1 )
		self.shader.use 	()
		self.shader.setTexture	( "noiseMap", 	0 )
		self.shader.setTexture	( "blueNoiseMap", 1 )
		self.shader.setUniformVec ( "eye", self.eye )

	def redisplay ( self ):
		glClear  	( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
		glDisable	( GL_DEPTH_TEST )

		self.shader.setUniformFloat ( "fov",  self.fov )
		self.shader.setUniformMat   ( "mv",   glm.mat3 ( self.getRotation () ) )
		self.shader.setUniformFloat ( "time", self.time () )
		self.mesh.render()

	def mouseScroll ( self, dx, dy ):
		self.fov += 0.05 * ( 1 if dy >= 0 else -1 )

def main():
	win = SDFWindow ( 600, 600, "Volumetric Cloud 4" )
	win.run ()

if __name__ == "__main__":
	main()
