import glfw
from OpenGL.GL import *
import OpenGL.GL.shaders
import numpy
from PIL import Image

def  _loadImage2D ( target, image ):
    print ( image.mode, image.width, image.height )
    if image.mode == 'L':			# handle paletted images
        image = image.convert ( mode = 'RGB' )
			
    img_data = numpy.array(list(image.getdata()), numpy.uint8)
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 )	# set 1-byte alignment

    if image.mode == 'RGB':
        glTexImage2D ( target, 0, GL_RGB,  image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data )
    else:		# RGBA
        glTexImage2D ( target, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data )

class    Texture:
    def __init__ ( self, filename = None, target = GL_TEXTURE_2D, clamp = GL_REPEAT, filter = GL_LINEAR, mipmaps = True ):
        self.filename = filename
        self.id       = glGenTextures ( 1 )
        self.target   = target
        self.clamp    = clamp
        self.filter   = filter
        glBindTexture ( self.target, self.id )

            # Set the texture wrapping parameters
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_S, clamp )
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_T, clamp )
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_R, clamp )

            # Set texture filtering parameters
        glTexParameteri ( self.target, GL_TEXTURE_MIN_FILTER, filter )
        glTexParameteri ( self.target, GL_TEXTURE_MAG_FILTER, filter )

            # load image
        if filename is not None:
            image       = Image.open ( filename )
            self.width  = image.width
            self.height = image.height
            _loadImage2D ( self.target, image )

            if mipmaps:
                glGenerateMipmap ( self.target )

    @classmethod
    def createEmpty ( cls, width, height, target = GL_TEXTURE_2D, intFormat = GL_RGBA8, format = GL_RGBA, clamp = GL_REPEAT, filter = GL_LINEAR ):
        tex        = Texture ( filename = None, target = target, clamp = clamp, filter = filter )
        tex.width  = width
        tex.height = height

        tex.bind ()
        glTexImage2D ( tex.target, 0, intFormat,  width, height, 0, format, GL_UNSIGNED_BYTE, None )

        return tex

    @classmethod
    def createEmpty3D ( cls, width, height, depth, intFormat = GL_RGBA8, format = GL_RGBA, clamp = GL_REPEAT, filter = GL_LINEAR ):
        tex        = Texture ( filename = None, target = GL_TEXTURE_3D, clamp = clamp, filter = filter )
        tex.width  = width
        tex.height = height
        tex.depth  = depth
		
        tex.bind ()
        glTexImage3D ( tex.target, 0, intFormat,  width, height, depth, 0, format, GL_UNSIGNED_BYTE, None )

        return tex
	
    def bind ( self, unit = 0 ):
        assert self.id != 0, "Texture not loaded"
        glActiveTexture ( GL_TEXTURE0 + unit )
        glBindTexture   ( self.target, self.id )

    def unbind ( self, unit = 0 ):
        assert self.id != 0, "Texture not loaded"
        glActiveTexture ( GL_TEXTURE0 + unit )
        glBindTexture   ( self.target, 0 )

    def buildMipmaps ( self ):
        assert self.id != 0 and self.width > 0 and self.height > 0

        glBindTexture    ( self.target, self.id )
        glTexParameteri  ( self.target, GL_TEXTURE_BASE_LEVEL, 0 )
        glGenerateMipmap ( self.target )
        glBindTexture    ( self.target, 0 )

class	CubeTexture:
    def __init__ ( self, names ):
        assert len(name) == 6, "For cube map we need exactly 6 images"
        self.id       = glGenTextures ( 1 )
        self.target   = GL_TEXTURE_CUBE_MAP
        glBindTexture ( self.target, self.id )

			# Set the texture wrapping parameters
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_S, GL_REPEAT )
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_T, GL_REPEAT )
        glTexParameteri ( self.target, GL_TEXTURE_WRAP_R, GL_REPEAT )

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
