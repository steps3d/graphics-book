import glfw
from OpenGL.GL import *
import OpenGL.GL.shaders
import numpy
from PIL import Image

def  _loadImage2D ( target, image ):
    if image.mode == 'L':			# handle paletted images
        image = image.convert ( mode = 'RGB' )
			
    img_data = numpy.array(list(image.getdata()), numpy.uint8)
    if image.mode == 'RGB':
        glTexImage2D ( target, 0, GL_RGB,  image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data )
    else:		# RGBA
        glTexImage2D ( target, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data )

class    Texture:
    def __init__ ( self, filename ):
        self.filename = filename
        self.id       = glGenTextures ( 1 )
        self.target   = GL_TEXTURE_2D
        glBindTexture ( self.target, self.id )

            # Set the texture wrapping parameters
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_S, GL_REPEAT )
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_T, GL_REPEAT )

            # Set texture filtering parameters
        glTexParameteri ( self.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR )
        glTexParameteri ( self.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR )

            # load image
        if filename is not None:
            image       = Image.open ( filename )
            self.width  = image.width
            self.height = image.height
            _loadImage2D ( self.target, image )
			 
            glGenerateMipmap ( self.target )

    @classmethod
    def createEmpty ( cls, width, height, intFormat = GL_RGBA8, format = GL_RGBA ):
        tex = Texture ( None )
        tex.width  = width
        tex.height = height

        tex.bind ()
        glTexImage2D ( tex.target, 0, intFormat,  width, height, 0, format, GL_UNSIGNED_BYTE, None )

        return tex

    def bind ( self, unit = 0 ):
        assert self.id != 0, "Texture not loaded"
        glActiveTexture ( GL_TEXTURE0 + unit )
        glBindTexture   ( self.target, self.id )

    def unbind ( self, unit = 0 ):
        assert self.id != 0, "Texture not loaded"
        glActiveTexture ( GL_TEXTURE0 + unit )
        glBindTexture   ( self.target, 0 )

class	CubeTexture:
    def __init__ ( self, names ):
        assert len(name) == 6, "For cube map we need exactly 6 images"
        self.id       = glGenTextures ( 1 )
        self.target   = GL_TEXTURE_CUBE_MAP
        glBindTexture ( self.target, self.id )

        # Set the texture wrapping parameters
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_S, GL_REPEAT )
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_T, GL_REPEAT )

        # Set texture filtering parameters
        glTexParameteri ( self.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR )
        glTexParameteri ( self.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR )

        # load image
        images = [Image.open ( name ) for name in names]
        self.width  = images [0].width
        self.height = images [0].height
        assert self.width == self.height

        for i in range ( 6 ):
            assert self.width == images [i].width and self.height == images [i].height
            _loadImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, images [i] )


        glGenerateMipmap ( self.target )

    def bind ( self, unit = 0 ):
        assert self.id != 0, "Texture not loaded"
        glActiveTexture ( GL_TEXTURE0 + unit )
        glBindTexture   ( self.target, self.id )
