import math
import glm
import numpy
from OpenGL.GL import *
import Window
import Program
import Texture
import Mesh

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye       = glm.vec3 ( -7, 0, 0 )
        self.lightDir  = glm.vec3 ( -1, 1, 1 )
        self.shader    = Program.Program ( glsl = "pbr.glsl" )
        print ( 'after program' )
        self.mesh      = Mesh.Mesh.createKnot ( 1, 4, 120, 30 )
        print ( 'after mesh' )
        self.albedo    = Texture.Texture ( 'rusted_iron/albedo.png' ) 
        print ( 'after tex0' )
        self.metalness = Texture.Texture ( 'rusted_iron/metallic.png' ) 
        print ( 'after tex1' )
        self.normal    = Texture.Texture ( 'rusted_iron/normal.png' ) 
        print ( 'after tex2' )
        self.roughness = Texture.Texture ( 'rusted_iron/roughness.png' ) 
        print ( 'after tex3' )
		
        self.albedo.bind    ( 0 )
        self.metalness.bind ( 1 )
        self.normal.bind    ( 2 )
        self.roughness.bind ( 3 )

        print ( 'after tex binds' )
		
        self.shader.use ()

        self.shader.setUniformVec ( "lightDir",   glm.vec3 ( -1, 1, 1 ) )
        #self.shader.setUniformVec ( "lightColor", glm.vec3 ( 4, 4, 4 ) )
		
        print ( 'after set uniforms' )

        self.shader.setTexture    ( "albedoMap",     0 )
        self.shader.setTexture    ( "metallnessMap", 1 )
        self.shader.setTexture    ( "normalMap",     2 )
        self.shader.setTexture    ( "roughnessMap",  3 )

        print ( 'after set textures' )

        self.idle ()

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv", self.getRotation () )
        self.shader.setUniformMat ( "nm", self.normalMatrix ( self.getRotation () ) )
        self.mesh.render()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )
        #self.shader.setUniformVec ( "eye",   self.eye )
        #self.shader.setUniformVec ( "light", self.light )

    def mouseScroll ( self, dx, dy ):
        #print ( 'wheel', dx, dy )
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

    def idle ( self ):
        angle = 4 * self.time ()
        self.light = glm.vec3 ( 8*math.cos(angle), 8*math.sin(1.4*angle), 8+0.5*math.sin (angle/3) )
        self.shader.setUniformVec ( "eye",   self.eye   )
        #self.shader.setUniformVec ( "light", self.light )


def main():
    win = MyWindow ( 900, 900, "Physically Based Rendering (PBR)" )

    if not win:
        glfw.terminate()
        return

    win.run ()

if __name__ == "__main__":
    main()


