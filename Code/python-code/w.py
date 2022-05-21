import sys
import glfw
from OpenGL.GL import *

if not glfw.init():
    print ( 'GLFW initialization error' )
    sys.exit ( 1 )

glfw.window_hint ( glfw.CONTEXT_VERSION_MAJOR, 3 )
glfw.window_hint ( glfw.CONTEXT_VERSION_MINOR, 3 )
glfw.window_hint ( glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE )
glfw.window_hint ( glfw.OPENGL_FORWARD_COMPAT, glfw.TRUE )
glfw.window_hint ( glfw.RED_BITS,     8 )
glfw.window_hint ( glfw.GREEN_BITS,   8 )
glfw.window_hint ( glfw.BLUE_BITS,    8 )
glfw.window_hint ( glfw.ALPHA_BITS,   8 )
glfw.window_hint ( glfw.DEPTH_BITS,   24 )

window = glfw.create_window ( 800, 600, "Test pyOpenGL window", None, None )
glfw.make_context_current ( window )

glViewport ( 0, 0, 800, 600 )

def display ():
    glClear ( GL_COLOR_BUFFER_BIT )

while not glfw.window_should_close ( window ):
    glfw.poll_events  ()
    display    ()
    glfw.swap_buffers ( window )
    
glfw.terminate ()
