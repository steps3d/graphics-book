import sys
import numpy        # needed to pass arrays to OpenGL
import glfw
from OpenGL.GL import *

def createBuffers ():
    vao = glGenVertexArrays ( 1 )
    glBindVertexArray ( vao )

    vertices = ( (-1, 0), (1, 0), (0,1) )
    vbo      = glGenBuffers ( 1 )
    glBindBuffer ( GL_ARRAY_BUFFER, vbo )
    glBufferData ( GL_ARRAY_BUFFER, (len(vertices))*2*4, numpy.array ( vertices, dtype = numpy.float32), GL_STATIC_DRAW )
    
    glVertexAttribPointer     ( 0, 2, GL_FLOAT, GL_FALSE, 2*4, ctypes.c_void_p(0))
    glEnableVertexAttribArray ( 0 )
    
    return vao


	# function to render image
def display ( vao ):
    glClear           ( GL_COLOR_BUFFER_BIT )
    glBindVertexArray ( vao )
    glDrawArrays      ( GL_TRIANGLES, 0, 3 )

	# key callback
def keyCallback ( window, key, scancode, action, mods ):
    if key in ( glfw.KEY_ESCAPE, 81, 113 ):        # Esc or q or Q
        #sys.exit ( 0 )
        glfw.set_window_should_close ( window, glfw.TRUE )

def sizeCallback ( window, width, height ):
    glViewport ( 0, 0, width, height )

	# initialize GLFW
if not glfw.init():
    print ( 'GLFW initialization error' )
    sys.exit ( 1 )

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

vao = createBuffers ()

	# event loop
while not glfw.window_should_close ( window ):
    glfw.poll_events  ()
    display    ( vao )
    glfw.swap_buffers ( window )

	#finalize    
glfw.terminate ()
