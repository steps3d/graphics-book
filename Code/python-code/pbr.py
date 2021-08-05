import glm
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
        self.mesh      = Mesh.Mesh.createKnot ( 1, 4, 120, 30 )
        self.albedo    = Texture.Texture ( 'rusted_iron/albedo.png' )
        self.metalness = Texture.Texture ( 'rusted_iron/metallic.png' )
        self.normal    = Texture.Texture ( 'rusted_iron/normal.png' )
        self.roughness = Texture.Texture ( 'rusted_iron/roughness.png' )

        self.albedo.bind    ( 0 )
        self.metalness.bind ( 1 )
        self.normal.bind    ( 2 )
        self.roughness.bind ( 3 )

        self.shader.use ()
        self.shader.setUniformVec ( "lightDir",   glm.vec3 ( -1, 1, 1 ) )
        self.shader.setTexture    ( "albedoMap",     0 )
        self.shader.setTexture    ( "metallnessMap", 1 )
        self.shader.setTexture    ( "normalMap",     2 )
        self.shader.setTexture    ( "roughnessMap",  3 )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv",  self.getRotation () )
        self.shader.setUniformMat ( "nm",  self.normalMatrix ( self.getRotation () ) )
        self.shader.setUniformVec ( "eye", self.eye   )
        self.mesh.render          ()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

def main():
    win = MyWindow ( 900, 900, "Physically Based Rendering (PBR)" )
    win.run ()

if __name__ == "__main__":
    main()
