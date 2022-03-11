'''
 Font module to output text using SDF fonts
'''

from OpenGL.GL import *
#import OpenGL.GL.shaders
#import glm
#import glfw
import numpy
#import Screen
import Program
import Texture
import Buffer

def _appendVertices ( vertices, *values ):
    for v in values:
        vertices.append ( v )

def _nextWord ( s, pos ):
    while pos < len ( s ) and s [pos] == ' ':
        pos += 1

    start = pos
    while pos < len ( s ) and s [pos] != ' ':
        pos += 1

    return s [start:pos], pos

def _nextValuePair ( s, pos ):
    sw, pos = _nextWord ( s, pos )
    i       = sw.find ( '=' )
    return int ( sw [i + 1:] ), pos

class Font:
    class BmChar:
        def __init__ ( self, x, y, width, height, xoffset, yoffset, xadvance, page ):
            self.x        = x
            self.y        = y
            self.width    = width
            self.height   = height
            self.xoffset  = xoffset
            self.yoffset  = yoffset
            self.xadvance = xadvance
            self.page     = page

    def __init__ ( self, fntFile, fntTex ):
        self.fontTex     = Texture.Texture ( fntTex )
        self.chars       = dict ()
        self.quadIndices = [0, 1, 2, 2, 3, 0]
        self.vertices    = []
        self.indices     = []
        self.program     = Program.Program ( glsl = 'sdf.glsl' )
        self.vao         = 0
        self.vbo         = None
        self.ibo         = None

        self.loadFnt ( fntFile )

    def buildVertices ( self, pos, size, text ):
        posx           = pos.x
        posy           = pos.y
        indexOffs      = 0
        self.vertices  = []
        self.indices   = []

        for ch in text:
            info = self.chars [ord(ch)]

            if info.width == 0:
                info.width = self.base

            charw = info.width / self.base
            dimx  = size * charw
            charh = info.height / self.base
            dimy  = size * charh
            us    = info.x / self.scaleW
            ue    = (info.x + info.width) / self.scaleW
            te    = info.y / self.scaleH
            ts    = (info.y + info.height) / self.scaleH
            xo    = info.xoffset / self.base

            _appendVertices ( self.vertices, posx + dimx + xo,  posy + dimy, 0.0, ue, te )
            _appendVertices ( self.vertices, posx + xo,         posy + dimy, 0.0, us, te )
            _appendVertices ( self.vertices, posx + xo,         posy,        0.0, us, ts )
            _appendVertices ( self.vertices, posx + dimx + xo,  posy,        0.0, ue, ts )

            for i in self.quadIndices:
                self.indices.append ( indexOffs + i )

            indexOffs += 4
            posx      += size * info.xadvance / self.base

        if self.vbo is None:
            self.vbo = Buffer.Buffer ( GL_ARRAY_BUFFER, self.vertices )
        else:
            self.vbo.setData ( self.vertices )

        if self.ibo is None:
            self.ibo = Buffer.Buffer ( GL_ELEMENT_ARRAY_BUFFER, self.indices, dtype = numpy.uint32 )
        else:
            self.ibo.setData ( self.indices )

        if self.vao == 0:
            self.vao = glGenVertexArrays ( 1 )
            glBindVertexArray ( self.vao )

            self.vbo.bind ()
            self.ibo.bind ()

            # need pos
            glVertexAttribPointer     ( 0, 3, GL_FLOAT, GL_FALSE, 5*4, ctypes.c_void_p(0))
            glEnableVertexAttribArray ( 0 )

            #texCoord
            glVertexAttribPointer     ( 1, 2, GL_FLOAT, GL_FALSE, 5*4, ctypes.c_void_p(12))
            glEnableVertexAttribArray ( 1 )

    def render ( self ):
        if self.vao != 0:
            self.program.use  ()
            glBindVertexArray ( self.vao )
            glDrawElements    ( GL_TRIANGLES, len(self.indices), GL_UNSIGNED_INT, None )

    def loadFnt ( self, fntFile ):
        with open ( fntFile ) as file:
            for line in file:
                pos       = 0
                word, pos = _nextWord ( line, pos )

                if word == 'char':
                    id,       pos = _nextValuePair ( line, pos )
                    x,        pos = _nextValuePair ( line, pos )
                    y,        pos = _nextValuePair ( line, pos )
                    width,    pos = _nextValuePair ( line, pos )
                    height,   pos = _nextValuePair ( line, pos )
                    xoffset,  pos = _nextValuePair ( line, pos )
                    yoffset,  pos = _nextValuePair ( line, pos )
                    xadvance, pos = _nextValuePair ( line, pos )
                    page,     pos = _nextValuePair ( line, pos )

                    self.chars [id] = Font.BmChar ( x, y, width, height, xoffset, yoffset, xadvance, page )
                elif word == 'common':
                    self.lineHeight, pos = _nextValuePair ( line, pos )
                    self.base,       pos = _nextValuePair ( line, pos )
                    self.scaleW,     pos = _nextValuePair ( line, pos )
                    self.scaleH,     pos = _nextValuePair ( line, pos )
