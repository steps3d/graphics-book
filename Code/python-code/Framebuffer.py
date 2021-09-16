from OpenGL.GL import *
import Texture

class   Framebuffer:
    def __init__ ( self, width, height, depth = False, stencil = False ):
        assert width > 0 and height > 0

        self.width         = width
        self.height        = height
        self.colorBuffers  = []
        self.depthBuffer   = None
        self.stencilBuffer = None
        self.saveViewport  = None
        self.id            = glGenFramebuffers ( 1 )

        glBindFramebuffer ( GL_FRAMEBUFFER, self.id )

        if depth:
            self.depthFormat = GL_DEPTH_COMPONENT24
            self.depthBuffer = glGenRenderbuffers ( 1 )
            glBindRenderbuffer        ( GL_RENDERBUFFER, self.depthBuffer )
            glRenderbufferStorage     ( GL_RENDERBUFFER, self.depthFormat, width, height )
            glFramebufferRenderbuffer ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, self.depthBuffer )

        if stencil:
            self.stencilFormat = GL_STENCIL_INDEX8
            self.stencilBuffer = glGenRenderbuffers ( 1 )
            glBindRenderbuffer        ( GL_RENDERBUFFER, self.stencilBuffer )
            glRenderbufferStorage     ( GL_RENDERBUFFER, self.stencilFormat, width, height )
            glFramebufferRenderbuffer ( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, self.depthBuffer )

        glBindFramebuffer ( GL_FRAMEBUFFER, 0 )

    def attachTexture ( self, tex ):
        print ( 'Attaching', tex, type(tex) )
        assert self.id != 0, "Must have a valid framebuffer object"
        assert self.width == tex.width and self.height == tex.height, "Texture size must match to framebuffer"

        no = len ( self.colorBuffers )
        self.colorBuffers.append ( tex )

        glBindTexture          ( tex.target, tex.id )
        glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, tex.target, tex.id, 0 )

        return no

    def createTexture ( self, intFormat = GL_RGBA8, format = GL_RGBA, clamp = GL_REPEAT, filter = GL_LINEAR ):
	    return Texture.Texture.createEmpty ( self.width, self.height, intFormat, format )
		
    def attachDepthTexture ( self, tex ):
        assert self.id != 0, "Must have a valid framebuffer object"
        assert self.width == tex.width and self.height == tex.height, "Texture size must match to framebuffer"

        #no = len ( self.colorBuffers )
        self.colorBuffers.append ( tex )

        glBindTexture        ( tex.target, tex.id )
        glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_DEPTH_ATTACHMENT, tex.id, 0 )

    def attachCubeMapFace ( self, tex, face,  ):
        assert self.id != 0, "Must have a valid framebuffer object"
        assert self.width == tex.width and self.height == tex.height, "Texture size must match to framebuffer"
        assert tex.target == GL_TEXTURE_CUBE_MAP
        assert face >= 0 and face <= 5, "Cube map face must be in 0..5 range"

        no = len ( self.colorBuffers )
        self.colorBuffers.append ( tex )

        glBindTexture        ( tex.target, tex.id )
        glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, tex.id, 0 )

    def bind ( self ):
        assert self.id != 0

        glFlush ()

        self.saveViewport = glGetIntegerv ( GL_VIEWPORT )       # save current viewport
        glBindFramebuffer ( GL_FRAMEBUFFER, self.id )
        glReadBuffer      ( GL_COLOR_ATTACHMENT0 )
        glDrawBuffer      ( GL_COLOR_ATTACHMENT0 )
        glViewport        ( 0, 0, self.width, self.height )

    def bindFace ( self, face ):
        glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, self.colorBufer [0].id, 0 )

    def unbind ( self ):
        assert self.id != 0
        assert self.saveViewport is not None

        glBindFramebuffer ( GL_FRAMEBUFFER, 0 )
        glViewport        ( self.saveViewport [0], self.saveViewport [1], self.saveViewport [2], self.saveViewport [3] )

    def drawBuffers ( self, no ):
        buffers = tuple ( GL_COLOR_ATTACHMENT0 + i for i in range ( no ) )
        glDrawBuffer ( no, buffers )

    def buildMipmaps ( self, no ):
        assert self.id != 0
        assert no >= 0 and no < len ( self.colorBuffers )

        tex = self.colorBuffers [no]
        glBindTexture    ( tex.target, tex.id )
        glTexParameteri  ( tex.target, GL_TEXTURE_BASE_LEVEL, 0 )
        glGenerateMipmap ( tex.target )                        # should be bound
        glBindTexture    ( tex.target, 0 )
