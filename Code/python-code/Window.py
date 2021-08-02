import sys
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
    
    def __init__ (self, w, h, title ):
        if not Window.initialized:
            if not glfw.init():
                print ( 'GLFW initialization error' )
                sys.exit ( 1 )
                
            Window.initialized = True
            glfw.window_hint ( glfw.CONTEXT_VERSION_MAJOR, 3 )
            glfw.window_hint ( glfw.CONTEXT_VERSION_MINOR, 3 )
            glfw.window_hint ( glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE )
            glfw.window_hint ( glfw.OPENGL_FORWARD_COMPAT, glfw.TRUE )
   
        self.window = glfw.create_window(w, h, title, None, None)
        self.width  = w
        self.height = h
        self.title  = title
        self.screenshotCount = 1

        if not self.window:
            glfw.terminate()
            sys.exit ( 1 )
            
        glfw.set_window_user_pointer(self.window, self)
        glfw.make_context_current(self.window)
        glfw.set_window_size_callback(self.window, Window.sizeCallback)
        glfw.set_key_callback(self.window, Window.keyCallback)
        glfw.set_cursor_pos_callback(self.window, Window.mouseMoveCallback)
        glfw.set_cursor_enter_callback(self.window, Window.mouseEnterCallback)
        glfw.set_mouse_button_callback(self.window, Window.mouseButtonCallback)
        glfw.set_scroll_callback(self.window, Window.mouseScrollCallback)
        
    def reshape ( self, width, height ):
        self.width  = width
        self.height = height
		
        glViewport ( 0, 0, width, height );
        pass
        
    def key ( self, key, scancode, action, mods ):
        if key in (256, 81, 113):        # Esc or q or Q
            glfw.set_window_should_close ( self.window, glfw.TRUE )
        
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
        
    def setTitle ( self, title ):
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
        pixels = glReadPixels    ( 0, 0, self.getWidth (), self.getHeight(), \
                                   GL_RGB, GL_UNSIGNED_BYTE ) #, ptr )
        image = Image.fromstring("RGB", (self.width, self.height), pixels)
        image = image.transpose( Image.FLIP_TOP_BOTTOM)
        image.save(filename)
        
    def run ( self ):
        self.reshape ( self.width, self.height )           # it's not called before 1st render
        while not glfw.window_should_close ( self.window ):
            self.idle         ()
            glfw.poll_events  ()
            self.redisplay    ()
            glfw.swap_buffers ( self.window )
        glfw.terminate()
     
   
class RotationWindow (Window):
    def __init__ ( self, w, h, title ):
        super().__init__ ( w, h, title )
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
        return m

    def normalMatrix ( self, m ):
        return glm.inverseTranspose ( glm.mat3 ( m ) )
        
    def getProjection ( self, fovDegrees = 60.0, zNear = 0.1, zFar = 100.0 ):
        return glm.perspective ( glm.radians ( fovDegrees ), self.getAspect (), zNear, zFar ) * glm.lookAt ( self.eye, glm.vec3 ( 0, 0, 0 ), glm.vec3 ( 0, 0, 1 ) )

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
    
