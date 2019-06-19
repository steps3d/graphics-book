cmake_minimum_required (VERSION 2.6)
project (Chapter-9)

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

add_library(Core  ../Core/GlutWindow.cpp ../Core/TgaImage.cpp ../Core/Program.cpp ../Core/Data.cpp ../Core/stringUtils.cpp ../Core/VertexBuffer.cpp ../Core/BasicMesh.cpp ../Core/Texture.cpp ../Libs/SOIL/SOIL.c ../Libs/SOIL/stb_image_aug.c ../Libs/SOIL/image_helper.c ../Libs/SOIL/image_DXT.c ../Core/randUtils.cpp ../Core/Framebuffer.cpp ../Core/Camera.cpp  ../Core/ScreenQuad.cpp ../Core/MeshLoader.cpp )

add_executable(lambert lambert.cpp)
target_link_libraries(lambert Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(blinn-phong blinn-phong.cpp)
target_link_libraries(blinn-phong Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(phong phong.cpp)
target_link_libraries(phong Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(phong-normalised phong-normalised.cpp)
target_link_libraries(phong-normalised Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(oern-nayar oren-nayar.cpp)
target_link_libraries(oren-nayar Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(oren-nayar-opt oren-nayar-opt.cpp)
target_link_libraries(oren-nayar-opt Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(cook-torrance cook-torrance.cpp)
target_link_libraries(cook-torrance Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(ashikhmin-shirley ashikhmin-shirley.cpp)
target_link_libraries(ashikhmin-shirley Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(disney disney.cpp)
target_link_libraries(disney Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(ward ward.cpp)
target_link_libraries(ward Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(minnaert minnaert.cpp)
target_link_libraries(minnaert Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(lommel-seeliger lommel-seeliger.cpp)
target_link_libraries(lommel-seeliger Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(strauss strauss.cpp)
target_link_libraries(strauss Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(aniso aniso.cpp)
target_link_libraries(aniso Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )

add_executable(blinn-phong-normalized blinn-phong-normalized.cpp)
target_link_libraries( blinn-phong-normalized Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} )


install(TARGETS debug DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/.. )
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/*.glsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
