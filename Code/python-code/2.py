import sys
import glfw
from OpenGL.GL import *

	# function to render image
def display ():
    glClear ( GL_COLOR_BUFFER_BIT )
    glBegin ( GL_TRIANGLES )
    glColor3f  ( 1, 0, 0 )
    glVertex2f ( -1, -1 )
    glColor3f  ( 0, 1, 0 )
    glVertex2f ( 1, -1 )
    glColor3f  ( 0, 0, 1 )
    glVertex2f ( 1, 1 )
    glEnd ()

	# key callback
def keyCallback ( window, key, scancode, action, mods ):
    print(window, key, scancode, action, mods )
    if key in ( glfw.KEY_ESCAPE, 81, 113 ):        # Esc or q or Q
        #sys.exit ( 0 )
        glfw.set_window_should_close ( window, glfw.TRUE )

def sizeCallback ( window, width, height ):
    print ( "Size:", width, height )
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

	# event loop
while not glfw.window_should_close ( window ):
    glfw.poll_events  ()
    display    ()
    glfw.swap_buffers ( window )

	#finalize    
glfw.terminate ()
