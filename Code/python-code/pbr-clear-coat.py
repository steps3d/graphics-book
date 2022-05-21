import glm
from OpenGL.GL import *
import Window
import Program
import Texture
import Mesh

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.strength   = 1.0
        self.eye        = glm.vec3 ( -7, 0, 0 )
        self.lightDir   = glm.vec3 ( -1, 1, 1 )
        self.mesh       = Mesh.Mesh.createKnot ( 1, 4, 120, 30 )
        self.shader     = Program.Program ( glsl = "pbr-clear-coat.glsl" )

        self.shader.use           ()
        self.shader.setUniformVec ( "lightDir",   glm.vec3 ( -1, 1, 1 ) )
        self.updateCaption        ()

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat   ( "mv",         self.getRotation () )
        self.shader.setUniformMat   ( "nm",         self.normalMatrix ( self.getRotation () ) )
        self.shader.setUniformVec   ( "eye",        self.eye   )
        self.shader.setUniformFloat ( "strength",   self.strength )
        self.mesh.render          ()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

    def key ( self, key, scancode, action, mods ):
        if key == 334 and action and self.strength < 1:
            self.strength += 0.05
        elif key == 333 and action and self.strength > 0:
            self.strength -= 0.05
            
        super().key ( key, scancode, action, mods )
        self.updateCaption ()
        
    def updateCaption ( self ):
        self.setCaption ( f"Physically Based Rendering (PBR), strength: {self.strength:.2}" )

def main():
    win = MyWindow ( 900, 900, "Physically Based Rendering (PBR) components" )
    win.run ()

if __name__ == "__main__":
    main()
