# Settings for Makefile.nmake to build executables
# These are used only in Makefile.nmake, not for cmake
#
# Here the paths to installed under Windows libraries
# FREEGLUT - path to installed freeglut library
# GLEW     - path to installed GLEW library
# GLM      - path to installed GLM
# GLFW     - path to installed GLFW
# OpenCL/CUDA are not used in this book

LIBS_ROOT     = f:/Libraries
GPU_LIBS      = C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.1
FREEGLUT      = $(LIBS_ROOT)/freeglut-3.0.0
GLEW          = $(LIBS_ROOT)/glew-2.1.0
GLM           = $(LIBS_ROOT)/glm
GLFW          = $(LIBS_ROOT)/glfw-3.2
GLFW_LIB_PATH = $(GLFW)/lib-vc2012

OCL_LIB       = OpenCL.lib
OCL_INC_PATH  = "$(GPU_LIBS)/include/"
OCL_LIB_PATH  = "$(GPU_LIBS)/lib/Win32/"
CUDA_INC_PATH = "$(GPU_LIBS)/include/"
CUDA_LIB_PATH = "$(GPU_LIBS)/lib/Win32/"

INC = $(INC) -I../Core -I$(FREEGLUT)/include -I$(GLEW)/include -I$(GLFW)/include -I$(GLM) -Ilibs -I. -I$(OCL_INC_PATH)

!if defined (DEBUG)
DEBUG_COMPILE_FLAGS = /Zi /MT
DEBUG_LINK_FLAGS    = /DEBUG /DEBUGTYPE:CV /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:libcmt.lib 
#DEBUG_LINK_FLAGS    = /DEBUG /DEBUGTYPE:CV /NODEFAULTLIB:libc.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:libcmt.lib 
!else
DEBUG_COMPILE_FLAGS = /MT
DEBUG_LINK_FLAGS    = /NODEFAULTLIB:libc.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcmt.lib 
!endif

CC              = cl
LINK            = link
link            = link
OPENGL          = opengl32.lib freeglut_staticd.lib glew32s.lib

!if defined (STATIC)
#LLDLIBS  = $(LFLAGS) $(DEBUG_LINK_FLAGS) $(OPENGL) /LIBPATH:$(FREEGLUT)/lib/win32-vc2010 /LIBPATH:$(GLEW)/lib/Release/Win32 /LIBPATH:../Base/libs zlib.lib libpng.lib libjpeg.lib  /LIBPATH:$(OCL_LIB_PATH) /LIBPATH:"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib"
LLDLIBS  = $(LFLAGS) $(DEBUG_LINK_FLAGS) $(OPENGL) /LIBPATH:$(FREEGLUT)/lib/Debug /LIBPATH:$(GLEW)/lib/Release/Win32 /LIBPATH:../Base/libs zlib.lib libpng.lib libjpeg.lib  /LIBPATH:$(OCL_LIB_PATH) /LIBPATH:"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib" legacy_stdio_definitions.lib
CFLAGS   = $(CFLAGS) /EHsc $(DEBUG_COMPILE_FLAGS) -DWIN32 -D_CRT_SECURE_NO_DEPRECATE -DFREEGLUT_STATIC -DGLEW_STATIC
!else
#LLDLIBS  = $(LFLAGS) $(DEBUG_LINK_FLAGS) $(OPENGL) glew32.lib /LIBPATH:$(FREEGLUT)/lib/win32-vc2010 /LIBPATH:$(GLEW)/lib /LIBPATH:../Base/libs /LIBPATH:$(OCL_LIB_PATH) zlib.lib libpng.lib libjpeg.lib $(OCL_LIB)
LLDLIBS  = $(LFLAGS) $(DEBUG_LINK_FLAGS) $(OPENGL) glew32.lib /LIBPATH:$(FREEGLUT)/lib/Debug /LIBPATH:$(GLEW)/lib /LIBPATH:../Base/libs /LIBPATH:$(OCL_LIB_PATH) zlib.lib libpng.lib libjpeg.lib $(OCL_LIB)
CFLAGS   = $(CFLAGS) /EHsc $(DEBUG_COMPILE_FLAGS) -DWIN32 -D_CRT_SECURE_NO_DEPRECATE -DFREEGLUT_STATIC
!endif
