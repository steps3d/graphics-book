import sys
import numpy        # needed to pass arrays to OpenGL
import glfw
from OpenGL.GL import *
import OpenGL.GL.shaders
from PIL import Image

vertices = ( (-1, -1), (1, -1), (1,1), (-1, 1) )
indices  = ( 0, 1, 2, 0, 2, 3 )

def createBuffers ():
    vao = glGenVertexArrays ( 1 )
    glBindVertexArray ( vao )

    vbo      = glGenBuffers ( 1 )
    glBindBuffer ( GL_ARRAY_BUFFER, vbo )
    glBufferData ( GL_ARRAY_BUFFER, (len(vertices))*2*4, numpy.array ( vertices, dtype = numpy.float32), GL_STATIC_DRAW )
    
    ibo      = glGenBuffers ( 1 )
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, ibo )
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, (len(indices))*4, numpy.array ( indices, dtype = numpy.uint32), GL_STATIC_DRAW )
    
    glVertexAttribPointer     ( 0, 2, GL_FLOAT, GL_FALSE, 2*4, ctypes.c_void_p(0) )
    glEnableVertexAttribArray ( 0 )
    
    return vao

def compileShaderFile ( fileName, shaderType ):
    source = open ( fileName ).read ()
    source = str.encode ( source )
    
    return OpenGL.GL.shaders.compileShader ( source, shaderType )

def createProgram ( vertex, fragment ):
    vertexShader   = compileShaderFile ( vertex,   GL_VERTEX_SHADER )
    fragmentShader = compileShaderFile ( fragment, GL_FRAGMENT_SHADER )
    
    program = OpenGL.GL.shaders.compileProgram ( vertexShader, fragmentShader, validate = True )
    assert program > 0, "Invalid program"
    
    return program
    
def  loadImage2D ( fileName, clamp, filterMin, filterMag, genMips ):
    id     = glGenTextures ( 1 )
    image  = Image.open ( fileName )
    target = GL_TEXTURE_2D

    glBindTexture ( target, id )

    if image.mode == 'L':			# handle paletted images
        image = image.convert ( mode = 'RGB' )
    elif image.mode == 'P':	
        image = image.convert ( mode = 'RGB' )

    print ( fileName, image.mode, image.width, image.height )

    img_data = numpy.array ( list ( image.getdata () ), numpy.uint8 )
    
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 )	# set 1-byte alignment

    if image.mode == 'RGB':
        glTexImage2D ( target, 0, GL_RGB,  image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data )
    else:		# RGBA
        glTexImage2D ( target, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data )

    if genMips:
        glGenerateMipmap ( target )

    glTexParameteri ( target, GL_TEXTURE_WRAP_S, clamp )
    glTexParameteri ( target, GL_TEXTURE_WRAP_T, clamp )
    glTexParameteri ( target, GL_TEXTURE_WRAP_R, clamp )

        # Set texture filtering parameters
    glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, filterMin )
    glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, filterMag )

    return id

def setTexture ( program, name, unit ):
    loc = glGetUniformLocation ( program, name )
    glUseProgram ( program )
    glUniform1i  ( loc, unit )

	# function to render image
def display ( vao, program ):
    glClear           ( GL_COLOR_BUFFER_BIT )
    glUseProgram      ( program )
    glBindVertexArray ( vao )
    glDrawElements    ( GL_TRIANGLES, len ( indices ), GL_UNSIGNED_INT, ctypes.c_void_p(0) )

	# key callback
def keyCallback ( window, key, scancode, action, mods ):
    if key in ( glfw.KEY_ESCAPE, 81, 113 ):        # Esc or q or Q
        #sys.exit ( 0 )
        glfw.set_window_should_close ( window, glfw.TRUE )

def sizeCallback ( window, width, height ):
    glViewport ( 0, 0, width, height )

	# initialize GLFW
if not glfw.init ():
    print ( 'GLFW initialization error' )
    sys.exit ( 1 )

    # at last we set for modern OpenGL 3.3
glfw.window_hint ( glfw.CONTEXT_VERSION_MAJOR, 3 )
glfw.window_hint ( glfw.CONTEXT_VERSION_MINOR, 3 )
glfw.window_hint ( glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE )
glfw.window_hint ( glfw.OPENGL_FORWARD_COMPAT, glfw.TRUE )

glfw.window_hint ( glfw.RED_BITS,     8 )
glfw.window_hint ( glfw.GREEN_BITS,   8 )
glfw.window_hint ( glfw.BLUE_BITS,    8 )
glfw.window_hint ( glfw.ALPHA_BITS,   8 )
glfw.window_hint ( glfw.DEPTH_BITS,   24 )
glfw.window_hint ( glfw.STENCIL_BITS, 8  )
glfw.window_hint ( glfw.RESIZABLE,    GL_TRUE )

	# create window and choose as current context
window = glfw.create_window ( 800, 600, "Test pyOpenGL window", None, None )
glfw.make_context_current     ( window )

	# set key callback
glfw.set_key_callback ( window, keyCallback )

    # set mouse callback
glfw.set_window_size_callback ( window, sizeCallback )

	# setup viewport and color to clear window
glViewport ( 0, 0, 800, 600 )
glClearColor ( 0, 0, 0, 1 )

vao     = createBuffers ()
program = createProgram ( "2.vert", "2.frag" )
tex     = loadImage2D   ( "16.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, True )

setTexture ( program, "image", 0 )

	# event loop
while not glfw.window_should_close ( window ):
    glfw.poll_events  ()
    display    ( vao, program )
    glfw.swap_buffers ( window )

	#finalize    
glfw.terminate ()
