cmake_minimum_required (VERSION 2.6)
project (Chapter-6)

if (WIN32)
		# For Windows have to specify locations of all libs
		# Use all libs as static
	set ( LIBS_ROOT     "f:/Libraries" )
	set ( GPU_LIBS      "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v9.2" )
	set ( FREEGLUT      "${LIBS_ROOT}/freeglut-3.0.0" )
	set ( GLEW          "${LIBS_ROOT}/glew-2.1.0" )
	set ( GLM           "${LIBS_ROOT}/glm" )
	set ( GLFW          "${LIBS_ROOT}/glfw-3.2" )
	set ( GLFW_LIB_PATH "${GLFW}/lib-vc2012" )
	set ( ASSIMP_DIR    "${LIBS_ROOT}/assimp-3.3.1" )
	set ( GLEW_LIB      glew32s )
	set ( GL_LIB	    opengl32 )
	set ( GLUT_LIB      )

	include_directories ( ${FREEGLUT}/include ${GLEW}/include ${GLM} ${GLFW} ${ASSIMP_DIR}/include )
	link_directories    ( ${FREEGLUT}/Lib/Debug ${FREEGLUT}/Lib/Release ${GLEW}/lib/Release/Win32 )
	
	 add_definitions(-D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DFREEGLUT_STATIC )
else ()
	set ( GLEW_LIB  GLEW )
	set ( GL_LIB	GL )
	set ( GLUT_LIB  glut )
endif ()

add_executable(test test.cpp)


