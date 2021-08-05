"""
    Wrapper for OpenGL shader program.
    Can load from separate shader files or a composite .glsl file
"""

from OpenGL.GL import *
import OpenGL.GL.shaders
import glm

class    Program:
    def __init__ ( self, **kwargs ):
        if "glsl" in kwargs:        # get all shaders from one file using -- syntax
            shaders = self.loadFromComposite ( kwargs ["glsl"] )
        else:
            self.vertexShader   = self.compileShader ( kwargs["vertex"],   GL_VERTEX_SHADER )
            self.fragmentShader = self.compileShader ( kwargs["fragment"], GL_FRAGMENT_SHADER )
            shaders             = [self.vertexShader, self.fragmentShader]
            if "geometry" in kwargs:
                self.geometryShader = self.compileShader ( kwargs["geometry"], GL_GEOMETRY_SHADER )
                shaders.append ( self.geometryShader )
            else:
                self.geometryShader = 0

        self.program = OpenGL.GL.shaders.compileProgram ( *shaders )
        assert self.program > 0, "Invalid program"

    def loadFromComposite ( self, filename ):
        data = dict ()                  # shader type to GL type and source
        curr = None                     # no active type
        with open ( filename ) as file:
            for line in file:
                if line.lstrip ().rstrip ().startswith ( "--" ):        # new shader startswith
                    curr = line.lstrip ().rstrip () [2:].lower ().split () [0]
                    assert curr in ("vertex", "fragment", "geometry"), "Invalid shader type"
                    data [curr] = ''
                else:
                    assert curr is not None, "Missing shader type"
                    data [curr] = data [curr] + line

        self.vertexShader   = self.compileShaderSource ( data ["vertex"],   GL_VERTEX_SHADER   )
        self.fragmentShader = self.compileShaderSource ( data ["fragment"], GL_FRAGMENT_SHADER )
        return [self.vertexShader, self.fragmentShader]

    def compileShaderSource ( self, source, shaderType ):
        source = str.encode ( source )
        return OpenGL.GL.shaders.compileShader ( source, shaderType )

    def compileShader ( self, fileName, shaderType ):
        #source = ""
        with open ( fileName ) as file:
            source = file.read()
        return self.compileShaderSource ( source, shaderType )

    def use ( self ):
        glUseProgram ( self.program )

    def __enter__ ( self ):
        glUseProgram ( self.program )

    def __exit__(self, tp, value, tb):
        glUseProgram ( 0 )

    def setTexture ( self, name, unit ):
        #assert self.program > 0, "Invalid program"
        loc = glGetUniformLocation ( self.program, name )
        #print ( self.program, loc, name )
        #assert loc >=0, f"Uniform {name} not found"

        glUniform1i(loc, unit)

    def setUniformFloat ( self, name, value ):
        loc = glGetUniformLocation ( self.program, name )
        #assert loc >=0, f"Uniform {name} not found"
        glUniform1f(loc, value)

    def setUniformInt ( self, name, value ):
        loc = glGetUniformLocation ( self.program, name )
        #assert loc >=0, f"Uniform {name} not found"
        glUniform1i(loc, value)

    def setUniformVec ( self, name, value ):
        loc = glGetUniformLocation ( self.program, name )
        #assert loc >=0, f"Uniform {name} not found"

        if isinstance(value, glm.vec2):
            glUniform2fv(loc, 1, value.to_tuple())
        elif isinstance(value, glm.vec3):
            glUniform3fv(loc, 1, value.to_tuple())
        elif isinstance(value, glm.vec4):
            glUniform4fv(loc, 1, value.to_tuple())
        elif isinstance(value, (tuple, list)):
            if len(value) == 2:
                glUniform2fv(loc, 1, value)
            elif len(value) == 3:
                glUniform3fv(loc, 1, value)
            elif len(value) == 4:
                glUniform4fv(loc, 1, value)

    def setUniformMat ( self, name, value, transpose = False ):
        loc = glGetUniformLocation ( self.program, name )
        #assert loc >=0, f"Uniform {name} not found"

        if isinstance(value, glm.mat4):
            glUniformMatrix4fv ( loc, 1, GL_TRUE if transpose else GL_FALSE, value.to_tuple() )
        elif isinstance(value, glm.mat3):
            glUniformMatrix3fv ( loc, 1, GL_TRUE if transpose else GL_FALSE, value.to_tuple() )
        elif isinstance(value, glm.mat2):
            glUniformMatrix2fv ( loc, 1, GL_TRUE if transpose else GL_FALSE, value.to_tuple() )
        else:
            assert True, f"Invalid matrix type {type(value)}"
