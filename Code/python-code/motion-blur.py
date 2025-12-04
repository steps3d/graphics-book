import math
import random
import glm
import numpy
from OpenGL.GL import *
import Window
import Program
import Texture
import Mesh
import Framebuffer
import Screen

class   AoWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.texture  = Texture.Texture ( "../../Textures/Fieldstone.dds" )
        self.texture2 = Texture.Texture ( '../../Textures/block.jpg' )
        self.eye      = glm.vec3 ( 0.2, 0.2, 0.2 )	#glm.vec3  ( 2.5, 2.5, 2.5 )
        self.boxes    = []
        self.screen   = Screen.Screen ()
        self.rndTex   = self.createRandomTexture ( 32 )
        self.shader   = Program.Program ( glsl = "gtao-decal.glsl" )
        self.shader.use         ()
        self.shader.setTexture  ( "image", 0 )
        self.fb    = Framebuffer.Framebuffer ( self.getWidth (), self.getHeight (), depth = True )
        self.fb.bind          ()
        self.fb.attachTexture ( self.fb.createTexture ( intFormat = GL_RGBA16F ) )
        self.fb.attachTexture ( self.fb.createTexture ( intFormat = GL_RGBA16F ) )
        self.fb.attachTexture ( self.fb.createTexture ( intFormat = GL_RGBA16F ) )
        self.fb.unbind        ()
        self.fb2   = Framebuffer.Framebuffer ( self.getWidth (), self.getHeight (), depth = False )
        self.fb2.bind          ()
        self.fb2.attachTexture ( self.fb2.createTexture ( intFormat = GL_RGBA16F ) )
        self.fb2.unbind        ()
        self.fb3   = Framebuffer.Framebuffer ( self.getWidth (), self.getHeight (), depth = False )
        self.fb3.bind          ()
        self.fb3.attachTexture ( self.fb3.createTexture ( intFormat = GL_RGBA16F ) )
        self.fb3.unbind        ()
        self.shader2    = None
        self.mvPrev     = None
        #self.blurStep   = glm.vec2 ( 1.0 / w, 1.0 / h )
        self.blurShader = Program.Program ( glsl = "ssao-blur.glsl" )
        self.blurShader.use        ()
        self.blurShader.setTexture ( "aoMap", 0 )
        self.blurShader.setTexture ( "nzMap", 1 )
        #self.blurShader.setUniformVec ( "step", self.blurStep )
        self.blurShader.setUniformFloat ( "sharpness", 1 )
        self.blurShader.unbind     ()
        self.createScene2 ( 10 )
        self.shader2  = Program.Program ( glsl = "motion-blur.glsl" )
        self.shader2.use         ()
        self.shader2.setTexture  ( "colorMap", 0 )
        self.shader2.setTexture  ( "nzMap",    1 )
        self.shader2.setTexture  ( "posMap",   2 )
        self.shader2.setTexture  ( "rndMap",   3 )
        #self.shader2.setUniformFloat ( "Radius", self.radius )
        self.shader2.unbind      ()


    def redisplay ( self ):
            # render to framebuffer ( color + nz textures )
        self.fb.bind ()
        glClearColor ( 0, 0, 0, 1 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        if self.mvPrev is None:
          self.mvPrev = self.getRotation ()

        self.shader.bind ()
        self.texture.bind ()
        self.shader.setUniformMat ( "mv",  self.getRotation () )
        self.shader.setUniformMat ( "mvPrevInv", glm.inverse ( self.mvPrev ) )
        self.shader.setUniformMat ( "nm",  self.normalMatrix ( self.getRotation () ) )
        self.shader.setUniformVec ( "eye", glm.vec3 ( 7, 7, 7 ) )

        self.box.render    ()
        self.texture2.bind ()

        for b in self.boxes:
            b.render ()

        self.texture.unbind ()
        self.shader.unbind  ()
        self.fb.unbind      ()

        glDisable    ( GL_DEPTH_TEST )
        glClear      ( GL_COLOR_BUFFER_BIT )

        self.fb.colorBuffers [0].bind ( 0 )   # color
        self.fb.colorBuffers [1].bind ( 1 )   # nz
        #self.fb.colorBuffers [2].bind ( 2 )
        self.rndTex.bind    ( 3 )
        self.shader2.bind   ()
        self.shader2.setUniformMat ( "mvPrevInv", glm.inverse ( self.mvPrev ) )
        self.shader2.setUniformMat ( "mv",     self.getRotation   () )
        self.shader2.setUniformMat ( "proj",   self.getProjection () )
        self.screen.render  ()
        self.shader2.unbind ()

        #if self.mvPrev != self.getRotation ():
        #  print ( self.mvPrev - self.getRotation () )

        self.mvPrev = self.getRotation ()


    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.bind ()
        self.shader.setUniformMat ( "proj",  self.getProjection () )
        self.shader.unbind ()
        self.shader2.bind ()
        self.shader2.setUniformMat ( "proj",  self.getProjection () )
        self.shader2.unbind ()

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )
        #print ( self.eye )

    def key ( self, key, scancode, action, mods ):
        if key == 32 and action:
            self.useBlur = not self.useBlur
        super().key ( key, scancode, action, mods )


    def createScene1 ( self, numBoxes, rotate = True ):
          # create box around the scene
        self.box = Mesh.Mesh.createBox ( glm.vec3 ( -7, -7, -7 ), glm.vec3 ( 12 ), True )

          # create random boxes inside the scene
        step = 1
        nx   = 12 // step
        ny   = 12 // step
        for x in range ( nx ):
            for y in range ( ny ):
                pos  = glm.vec3 ( -7 + step*x, -4, -7 + step*y )
                size = glm.vec3 ( step, random.uniform ( 0.5, 2 ), step )
                self.boxes.append ( Mesh.Mesh.createBox ( pos = pos, size = size ) )

    def createScene2 ( self, numBoxes, rotate = True ):
          # create box around the scene
        self.box = Mesh.Mesh.createBox ( glm.vec3 ( -7, -7, -7 ), glm.vec3 ( 12 ), True )

          # create random boxes inside the scene
        for i in range ( numBoxes ):
          pos  = glm.vec3 ( random.uniform ( -5, 0 ), random.uniform ( -5, 5 ), random.uniform ( -5, 5 ) )
          rot  = glm.normalize ( glm.vec3 ( random.uniform ( -1, 1 ), random.uniform ( -1, 1 ), random.uniform ( -1, 1 ) ) )
          ang  = random.uniform ( 0, 180 ) if rotate else 0
          mat  = glm.mat3 ( glm.rotate ( glm.mat4(1), glm.radians(ang), rot ) )
          size = glm.vec3 ( random.uniform ( 0.1, 1.2 ), random.uniform ( 0.1, 1.2 ), random.uniform ( 0.1, 1.2 ) )
          self.boxes.append ( Mesh.Mesh.createBox ( -0.5*size, size, mat = mat, offs = pos ) )
          
    def createRandomTexture ( self, n ):
        data = numpy.random.randint ( 0, 256, size = ( n, n, 4 ), dtype = numpy.uint8 )

        return Texture.Texture.createWithData ( n, n, data )

def main():
    win = AoWindow ( 900, 900, "Motion Blur" )
    win.run ()

if __name__ == "__main__":
    main()

