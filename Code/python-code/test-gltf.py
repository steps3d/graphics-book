import os
import os.path
import math
import glm
import numpy
from OpenGL.GL import *				# glBindVertexArray
import Window
import Program
import Texture
import Mesh
import Buffer
import loadGltf

class MyWindow(Window.RotationWindow):
    def __init__(self, w, h, t, filename):
        super().__init__(w, h, t)
        self.eye = glm.vec3(-7, 0, 0)
        self.model  = loadGltf.loadGltf ( filename )
        self.shader = Program.Program(glsl = "pbr-gltf.glsl")
        self.shader.use()
        self.shader.setTexture('albedoMap', 0)
        self.shader.setTexture('normalMap', 1)
        self.shader.setTexture('mrMap',     2)
        self.bones = [glm.mat4 ( 1 ), glm.mat4 ( 1 ), glm.mat4 ( 1 )]
        self.light = glm.vec3 ( 0 )

    def redisplay(self):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.model.nodes [0].render ( self.getRotation(), self.shader )

    def reshape(self, width, height):
        super().reshape(width, height)
        self.shader.setUniformMat("proj", self.getProjection(zFar=25000))
        self.shader.setUniformVec("eye", self.eye)

    def mouseScroll(self, dx, dy):
        self.eye += glm.vec3(0.3 * (1 if dy >= 0 else -1))
        self.reshape(self.width, self.height)

#filename = 'glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf'
#filename = 'model.weapons/Beretta-92/scene.gltf'
filename = 'model.weapons/ppsh/scene.gltf'
MyWindow ( 800, 600, "Loading GLTF model", filename ).run ()
