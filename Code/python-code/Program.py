"""
    Wrapper for OpenGL shader program.
    Can load from separate shader files or a composite .glsl file
"""

#from OpenGL.GL import *
from OpenGL.GL import GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_COMPUTE_SHADER, GL_FALSE, GL_TRUE, GL_FLOAT
from OpenGL.GL import glUseProgram, glGetUniformLocation,glUniform1i, glUniform1f   #, glUniform2f, glUniform3f, glUniform4f
from OpenGL.GL import  glUniform2fv, glUniform3fv, glUniform4fv, glUniformMatrix2fv, glUniformMatrix3fv, glUniformMatrix4fv
from OpenGL.GL import  glVertexAttribPointer, glVertexAttribIPointer, glEnableVertexAttribArray
import OpenGL.GL.shaders
import glm

class    Program:
    types = {"vertex" : GL_VERTEX_SHADER, "fragment" : GL_FRAGMENT_SHADER, "geometry": GL_GEOMETRY_SHADER,
            "tesscontrol": GL_TESS_CONTROL_SHADER, "tesseval": GL_TESS_EVALUATION_SHADER, "compute": GL_COMPUTE_SHADER}
            
    def __init__ ( self, validate = False, **kwargs ):
        if "glsl" in kwargs:        # get all shaders from one file using -- syntax
            shaders = self.loadFromComposite ( kwargs ["glsl"] )
        elif ("vertex_source" in kwargs) or ("fragment_source" in kwargs):
            self.vertexShader   = self.compileShaderSource ( kwargs["vertex_source"],   GL_VERTEX_SHADER )
            self.fragmentShader = self.compileShaderSource ( kwargs["fragment_source"], GL_FRAGMENT_SHADER )
            shaders             = [self.vertexShader, self.fragmentShader]

            if "geometry_source" in kwargs:
                self.geometryShader = self.compileShaderSource ( kwargs["geometry_source"], GL_GEOMETRY_SHADER )
                shaders.append ( self.geometryShader )
            else:
                self.geometryShader = 0
            # ?????
        else:
            self.vertexShader   = self.compileShaderFile ( kwargs["vertex"],   GL_VERTEX_SHADER )
            self.fragmentShader = self.compileShaderFile ( kwargs["fragment"], GL_FRAGMENT_SHADER )
            shaders             = [self.vertexShader, self.fragmentShader]

            if "geometry" in kwargs:
                self.geometryShader = self.compileShaderFile ( kwargs["geometry"], GL_GEOMETRY_SHADER )
                shaders.append ( self.geometryShader )
            else:
                self.geometryShader = 0

            if "tesscontrol" in kwargs:
                self.tessControlShader = self.compileShaderFile ( kwargs["tesscontrol"], GL_TESS_CONTROL_SHADER )
                shaders.append ( self.tessControlShader )
            else:
                self.tessControlShader = 0

            if "tesseval" in kwargs:
                self.tessEvalShader = self.compileShaderFile ( kwargs["tesseval"], GL_TESS_EVALUATION_SHADER )
                shaders.append ( self.tessEvalShader )
            else:
                self.tessEvalShader = 0

        self.program = OpenGL.GL.shaders.compileProgram ( *shaders, validate = validate )
        assert self.program > 0, "Invalid program"

    def loadFromComposite ( self, filename, defines = None ):
        data = dict ()                  # shader type to GL type and source
        curr = None                     # no active type
        
        with open ( filename ) as file:
            for line in file:

                if line.lstrip ().startswith ( '//' ) or line.lstrip () == "":
                    continue
                    
                if line.lstrip ().startswith ( '#include' ):

                        # get file name and dequote it
                    file = line.split ()[1][1:-2]
                    with open ( file ) as f:
                        for ln in f:
                            data [curr] = data [curr] + ln

                    continue

                if line.lstrip ().startswith ( '#version' ) and defines:
                        # add defines
                    for k in defines:
                        res = res + f'#define {k}\t\t{defines[k]}\n'
            
                if line.lstrip ().rstrip ().startswith ( "--" ):        # new shader startswith
                    curr = line.lstrip ().rstrip () [2:].lower ().split () [0]
                    assert curr in Program.types, f"Invalid shader type {curr}"
                    data [curr] = ''
                else:
                    assert curr is not None, "Missing shader type"
                    data [curr] = data [curr] + line

        shaders = []
        
        for sh in data:
            shader = self.compileShaderSource ( data [sh], Program.types [sh], defines )
            shaders.append ( shader )
            if sh == "vertex":
                self.vertexShader = shader
            elif sh == "fragment":
                self.fragmentShader = shader
            elif sh == "geometry":
                self.geometryShader = shader
            elif sh == "tesscontrol":
                self.tessControlShader = shader
            elif sh == "tesseval":
                self.tessEvalShader = shader
        
        return shaders

    def compileShaderFile ( self, fileName, shaderType, defines = None ):
        source = open ( fileName ).read ()
        return self.compileShaderString ( self.processShaderSource ( source, defines ), shaderType )
        
    def compileShaderSource ( self, source, shaderType, defines = None ):
        return self.compileShaderString ( self.processShaderSource ( source, defines ), shaderType )

    def processShaderSource ( self, source, defines = None ):
        res = ''
        for line in source.split ( '\n' ):
            if line.lstrip ().startswith ( '#include' ):

                    # get file name and dequote it
                file = line.split ()[1][1:-2]
                with open ( file ) as f:
                    for ln in f:
                        res = res + ln

                continue

            if line.lstrip ().startswith ( '#version' ) and defines:
                    # add defines
                for k in defines:
                    res = res + f'#define {k}\t\t{defines[k]}\n'
                # add line as it is
            res = res + line + '\n'
        return res

    def compileShaderString ( self, source, shaderType ):
        source = str.encode ( source )
        return OpenGL.GL.shaders.compileShader ( source, shaderType )

    def use ( self ):
        glUseProgram ( self.program )

    def bind ( self ):
        glUseProgram ( self.program )

    def unbind ( self ):
        glUseProgram ( 0 )

    def __enter__ ( self ):
        glUseProgram ( self.program )

    def __exit__(self, tp, value, tb):
        glUseProgram ( 0 )

    def setTexture ( self, name, unit ):
        loc = glGetUniformLocation ( self.program, name )
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
        if loc < 0:
            return

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
        if loc < 0:
            return

        if isinstance(value, glm.mat4):
            glUniformMatrix4fv ( loc, 1, GL_TRUE if transpose else GL_FALSE, value.to_tuple() )
        elif isinstance(value, glm.mat3):
            glUniformMatrix3fv ( loc, 1, GL_TRUE if transpose else GL_FALSE, value.to_tuple() )
        elif isinstance(value, glm.mat2):
            glUniformMatrix2fv ( loc, 1, GL_TRUE if transpose else GL_FALSE, value.to_tuple() )
        else:
            assert True, f"Invalid matrix type {type(value)}"

    def setAttrPtr ( self, name, numComponents, stride, offs, attrType = GL_FLOAT, normalized = False ):
        loc = glGetUniformLocation ( self.program, name )
        if loc < 0:
            return
        glVertexAttribPointer ( loc,                       # index
                                numComponents,             # number of values per vertex
                                attrType,                  # type (GL_FLOAT)
                                GL_TRUE if normalized else GL_FALSE,
                                stride,                    # stride (offset to next vertex data)
                                offs )
        
        glEnableVertexAttribArray ( loc )

    def setIntAttrPtr ( self, name, numComponents, stride, offs, attrType, normalized = False ):
        loc = glGetUniformLocation ( self.program, name )
        if loc < 0:
            return
        glVertexAttribIPointer ( loc,                       # index
                                numComponents,              # number of values per vertex
                                attrType,                   # type (GL_FLOAT)
                                stride,                     # stride (offset to next vertex data)
                                offs )
        
        glEnableVertexAttribArray ( loc )

