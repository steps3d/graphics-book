cmake_minimum_required (VERSION 2.6)
project (Chapter-12)

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
	set ( GL_LIB		opengl32 )
	set ( GLUT_LIB      )

	include_directories ( ${FREEGLUT}/include ${GLEW}/include ${GLM} ${GLFW} ${ASSIMP_DIR}/include )
	link_directories    ( ${FREEGLUT}/Lib/Debug ${FREEGLUT}/Lib/Release ${GLEW}/lib/Release/Win32 )
	
	 add_definitions(-D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DFREEGLUT_STATIC )
else ()
	set ( GLEW_LIB GLEW )
	set ( GL_LIB		GL )
	set ( GLUT_LIB      glut )
endif ()

include_directories("../Core" "../Libs/SOIL")

add_library(Core  ../Core/GlutWindow.cpp ../Core/TgaImage.cpp ../Core/Program.cpp ../Core/Data.cpp ../Core/stringUtils.cpp ../Core/VertexBuffer.cpp ../Core/BasicMesh.cpp ../Core/Texture.cpp ../Libs/SOIL/SOIL.c ../Libs/SOIL/stb_image_aug.c ../Libs/SOIL/image_helper.c ../Libs/SOIL/image_DXT.c ../Core/randUtils.cpp ../Core/Framebuffer.cpp ../Core/Camera.cpp  ../Core/ScreenQuad.cpp ../Core/MeshLoader.cpp ../Core/bbox.cpp )

add_executable(test test.cpp)
target_link_libraries(test Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(test-classes test-classes.cpp)
target_link_libraries(test-classes Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(sphere sphere.cpp)
target_link_libraries(sphere Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )


if (WIN32)
	install(TARGETS test DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS test-classes DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS sphere DESTINATION  ${CMAKE_SOURCE_DIR} )
endif ()