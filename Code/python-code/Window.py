"""
    Window class wrapper for GLFW windows
"""

import sys
import os
import os.path
import glfw
import glm
from OpenGL.GL import *
from PIL import Image

class Window:
    initialized = False

    @classmethod
    def sizeCallback ( cls, window, width, height ):
        w = glfw.get_window_user_pointer(window)
        if w:
            w.width  = width
            w.height = height
            w.reshape ( width, height )

    @classmethod
    def keyCallback ( cls, window, key, scancode, action, mods ):
        w = glfw.get_window_user_pointer(window)
        if w:
            w.key ( key, scancode, action, mods )

    @classmethod
    def mouseMoveCallback ( cls, window, x, y ):
        w = glfw.get_window_user_pointer(window)
        if w:
            w.mouseMove ( x, y )

    @classmethod
    def mouseButtonCallback ( cls, window, button, action, mods ):
        w = glfw.get_window_user_pointer(window)
        if w:
            w.mouseButton ( button, action, mods )

    @classmethod
    def mouseEnterCallback ( cls, window, enter ):
        w = glfw.get_window_user_pointer(window)
        if w:
            w.mouseEnter ( enter )

    @classmethod
    def mouseScrollCallback ( cls, window, dx, dy ):
        w = glfw.get_window_user_pointer(window)
        if w:
            w.mouseScroll ( dx, dy )

    def __init__ (self, w, h, title, fullScreen = False, depth = True, stencil = False, resizable = True ):
        if not Window.initialized:
            if not glfw.init():
                print ( 'GLFW initialization error' )
                sys.exit ( 1 )

            Window.initialized = True
            glfw.window_hint ( glfw.CONTEXT_VERSION_MAJOR, 3 )
            glfw.window_hint ( glfw.CONTEXT_VERSION_MINOR, 3 )
            glfw.window_hint ( glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE )
            glfw.window_hint ( glfw.OPENGL_FORWARD_COMPAT, glfw.TRUE )

            glfw.window_hint ( glfw.RED_BITS,     8 )
            glfw.window_hint ( glfw.GREEN_BITS,   8 )
            glfw.window_hint ( glfw.BLUE_BITS,    8 )
            glfw.window_hint ( glfw.ALPHA_BITS,   8 )
            glfw.window_hint ( glfw.DEPTH_BITS,   24 if depth   else 0 )
            glfw.window_hint ( glfw.STENCIL_BITS, 8  if stencil else 0 )
            glfw.window_hint ( glfw.RESIZABLE,    GL_TRUE if resizable else GL_FALSE )

        if fullScreen:
            monitor     = glfw.get_primary_monitor ()
            mode        = glfw.get_video_mode      ( monitor )
            w           = mode.size.width
            h           = mode.size.height
            #self.window = glfw.create_window ( w, h, title, monitor, None )
        else:
            monitor     = None

        self.window          = glfw.create_window ( w, h, title, monitor, None )
        self.width           = w
        self.height          = h
        self.title           = title
        self.frameNo         = 0
        self.frameTime       = [0, 0, 0, 0, 0]
        self.fps             = 0.0
        self.showFps         = True
        self.screenshotCount = 1
        self.screenshotName  = 'screenshot'

        if len ( sys.argv ) > 0:
            self.screenshotName = os.path.splitext ( os.path.basename ( sys.argv [0] ) ) [0]
            print ( 'Set screenshot name to', self.screenshotName )

        if not self.window:
            glfw.terminate()
            print('Could not create window')
            sys.exit ( 1 )

        glfw.set_window_user_pointer   ( self.window, self                       )
        glfw.make_context_current      ( self.window                             )
        glfw.set_window_size_callback  ( self.window, Window.sizeCallback        )
        glfw.set_key_callback          ( self.window, Window.keyCallback         )
        glfw.set_cursor_pos_callback   ( self.window, Window.mouseMoveCallback   )
        glfw.set_cursor_enter_callback ( self.window, Window.mouseEnterCallback  )
        glfw.set_mouse_button_callback ( self.window, Window.mouseButtonCallback )
        glfw.set_scroll_callback       ( self.window, Window.mouseScrollCallback )

    def reshape ( self, width, height ):
        self.width  = width
        self.height = height

        glViewport ( 0, 0, width, height )

    def key ( self, key, scancode, action, mods ):
        if key in (glfw.KEY_ESCAPE, 81, 113):        # Esc or q or Q
            glfw.set_window_should_close ( self.window, glfw.TRUE )
        elif key == glfw.KEY_F1 and action == glfw.PRESS:
            screenshotName = f'{self.screenshotName}-{self.screenshotCount}.png'
            self.screenshotCount += 1
            self.makeScreenshot ( screenshotName )
            print ( 'Created screenshot', screenshotName )

    def mouseMove ( self, x, y ):
        pass

    def mouseEnter ( self, enter ):
        pass

    def mouseButton ( self, button, action, mods ):
        pass

    def mouseScroll ( self, dx, dy ):
        pass

    def redisplay ( self ):
        pass

    def idle ( self ):
        pass

    def time ( self ):
        return glfw.get_time ()

    def getSize ( self ):
        return ( self.width, self.height )

    def getWidth ( self ):
        return self.getSize () [0]

    def getHeight ( self ):
        return self.getSize () [0]

    def getAspect ( self ):
        size = self.getSize ()
        return size [0] / size [1]

    def getCaption ( self ):
        return self.title

    def setSize ( self, w, h ):
        glfw.set_window_size ( self.window, w, h )

    def setPos ( self, x, y ):
        glfw.set_window_pos ( self.window, x, y )

    def setCaption ( self, title ):
        self.title = title
        glfw.set_window_title ( self.window, title )

    def show ( self, flag = True ):
        if flag:
            glfw.show_window ( self.window )
        else:
            glfw.hide_window ( self.window )

    def makeScreenshot ( self, filename ):
        glPixelStorei   ( GL_PACK_ALIGNMENT, 1 )           # set 1-byte alignment
        glFinish        ()
        glReadBuffer    ( GL_FRONT )
        pixels = glReadPixels    ( 0, 0, self.width, self.height, \
                                   GL_RGB, GL_UNSIGNED_BYTE )
        image = Image.frombytes ( "RGB", (self.width, self.height), pixels )
        image = image.transpose  ( Image.FLIP_TOP_BOTTOM )
        image.save ( filename )

    def run ( self ):
        self.reshape ( self.width, self.height )           # it's not called before 1st render
        while not glfw.window_should_close ( self.window ):
            self.idle         ()
            glfw.poll_events  ()
            self.redisplay    ()
            glfw.swap_buffers ( self.window )
            self.updateFps    ()
        glfw.terminate()
    def updateFps ( self ):
        for i in range ( len ( self.frameTime ) - 1 ):
            self.frameTime [i] = self.frameTime [i+1]

        self.frameTime [-1] = self.time ()
        self.fps            = len ( self.frameTime ) / ( self.frameTime [-1] - self.frameTime [0] + 0.001 )
        self.frameNo       += 1

        if self.showFps and (self.frameNo % 5) == 0:
            glfw.set_window_title ( self.window, self.title + f' FPS {self.fps:.1f}' )

class RotationWindow (Window):
    def __init__ ( self, w, h, title, depth = True, stencil = False, resizable = True  ):
        super().__init__ ( w, h, title, depth = depth, stencil = stencil, resizable = resizable )
        self.mouseOldX       = 0
        self.mouseOldY       = 0
        self.rot             = glm.vec3 ( 0, 0, 0 )
        self.eye             = glm.vec3 ( 5, 5, 5 )
        self.rightButtonDown = False

    def moveEyeTo ( self, pos ):
        self.eye = pos
        self.reshape ( self.getWidth (), self.getHeight () )

    def getRotation ( self ):
        m = glm.mat4 ( 1 )
        m = glm.rotate ( m, glm.radians ( -self.rot.z ), glm.vec3 ( 0, 0, 1 ) )
        m = glm.rotate ( m, glm.radians (  self.rot.y ), glm.vec3 ( 0, 1, 0 ) )
        m = glm.rotate ( m, glm.radians (  self.rot.x ), glm.vec3 ( 1, 0, 0 ) )
        return glm.lookAt ( self.eye, glm.vec3 ( 0, 0, 0 ), glm.vec3 ( 0, 0, 1 ) ) * m

    def normalMatrix ( self, m ):
        return glm.inverseTranspose ( glm.mat3 ( m ) )

    def getProjection ( self, fovDegrees = 60.0, zNear = 0.1, zFar = 100.0 ):
        #return glm.perspective ( glm.radians ( fovDegrees ), self.getAspect (), zNear, zFar ) * glm.lookAt ( self.eye, glm.vec3 ( 0, 0, 0 ), glm.vec3 ( 0, 0, 1 ) )
        return glm.perspective ( glm.radians ( fovDegrees ), self.getAspect (), zNear, zFar )

    def mouseButton ( self, button, action, mods ):
        if button == glfw.MOUSE_BUTTON_LEFT and action == glfw.PRESS:
            self.rightButtonDown = True
            self.mouseOldX = None           # clear
            self.mouseOldY = None
        elif button == glfw.MOUSE_BUTTON_LEFT and action == glfw.RELEASE:
            self.rightButtonDown = False

    def mouseMove ( self, x, y ):
        if self.rightButtonDown:
            if self.mouseOldX is None or self.mouseOldY is None:
                self.mouseOldX = x
                self.mouseOldY = y
                return

            self.rot.x -= ((self.mouseOldY - y) * 180.0) / 200.0
            self.rot.z -= ((self.mouseOldX - x) * 180.0) / 200.0
            self.rot.y  = 0

            if self.rot.z > 360:
                self.rot.z -= 360

            if self.rot.z < -360:
                self.rot.z += 360

            if self.rot.y > 360:
                self.rot.y -= 360

            if self.rot.y < -360:
                self.rot.y += 360

            self.mouseOldX = x
            self.mouseOldY = y
