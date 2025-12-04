import sys
import glfw
from OpenGL.GL import *

	# initialize GLFW
if not glfw.init():
    print ( 'GLFW initialization error' )
    sys.exit ( 1 )

	# create window and choose as current context
window = glfw.create_window ( 800, 600, "Test pyOpenGL window", None, None )
glfw.make_context_current ( window )

	# setup viewport and color to clear window
glViewport ( 0, 0, 800, 600 )
glClearColor ( 0, 0, 0, 1 )

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

	# event loop
while not glfw.window_should_close ( window ):
    glfw.poll_events  ()
    display    ()
    glfw.swap_buffers ( window )

	#finalize    
glfw.terminate ()
