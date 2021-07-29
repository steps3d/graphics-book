import glfw
from OpenGL.GL import *
import OpenGL.GL.shaders
import numpy
#import pyrr
#import glm
from PIL import Image

class    Texture:
    def __init__ ( self, filename ):
        self.id       = glGenTextures ( 1 )
        self.filename = filename
        self.target   = GL_TEXTURE_2D
        glBindTexture ( self.target, self.id )
        # Set the texture wrapping parameters
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_S, GL_REPEAT )
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_T, GL_REPEAT )
        # Set texture filtering parameters
        glTexParameteri ( self.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR )
        glTexParameteri ( self.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR )
        # load image
        image       = Image.open ( filename )
        print ( 'Image:', filename, image.mode )
        if image.mode == 'L':			# handle paletted images
            image = image.convert ( mode = 'RGB' )
			
        img_data    = numpy.array(list(image.getdata()), numpy.uint8)
        self.width  = image.width
        self.height = image.height
        if image.mode == 'RGB':
             glTexImage2D ( self.target, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data )
        else:		# RGBA
             glTexImage2D ( self.target, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data )
			 
        glGenerateMipmap ( self.target )

    def bind ( self, unit = 0 ):
        assert self.id != 0, "Texture not loaded"
        glActiveTexture ( GL_TEXTURE0 + unit )
        glBindTexture   ( self.target, self.id )

