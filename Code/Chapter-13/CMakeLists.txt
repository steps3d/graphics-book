cmake_minimum_required (VERSION 2.6)
project (Chapter-13)

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
	set ( ASSIMP_LIB	assimp )
endif ()

include_directories("../Core" "../Libs/SOIL")

add_library(Core  ../Core/GlutWindow.cpp ../Core/TgaImage.cpp ../Core/Program.cpp ../Core/Data.cpp ../Core/stringUtils.cpp ../Core/VertexBuffer.cpp ../Core/BasicMesh.cpp ../Core/Texture.cpp ../Libs/SOIL/SOIL.c ../Libs/SOIL/stb_image_aug.c ../Libs/SOIL/image_helper.c ../Libs/SOIL/image_DXT.c ../Core/randUtils.cpp ../Core/Framebuffer.cpp ../Core/Camera.cpp  ../Core/ScreenQuad.cpp ../Core/MeshLoader.cpp ../Core/bbox.cpp )

add_executable(fur fur.cpp)
target_link_libraries(fur Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(glass glass.cpp)
target_link_libraries(glass Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(glass-2 glass-2.cpp)
target_link_libraries(glass-2 Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(embm embm.cpp)
target_link_libraries(embm Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(envmapping envmapping.cpp)
target_link_libraries(envmapping Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(reflection reflection.cpp)
target_link_libraries(reflection Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(fire fire.cpp)
target_link_libraries(fire Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(sdf sdf.cpp)
target_link_libraries(sdf Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(bumpmapping bumpmapping.cpp)
target_link_libraries(bumpmapping Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(explosion explosion.cpp)
target_link_libraries(explosion Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(pbr pbr.cpp)
target_link_libraries(pbr Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(projective projective.cpp)
target_link_libraries(projective Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(shadow-mapping shadow-mapping.cpp)
target_link_libraries(shadow-mapping Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(shadow-mapping-2 shadow-mapping-2.cpp)
target_link_libraries(shadow-mapping-2 Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(burn burn.cpp)
target_link_libraries(burn Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )

add_executable(burn-2 burn-2.cpp)
target_link_libraries(burn-2 Core ${GLEW_LIB} ${GL_LIB} ${GLUT_LIB} ${ASSIMP_LIB} )


if (WIN32)
	install(TARGETS fur DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS glass DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS glass-2 DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS embm DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS envmapping DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS reflection DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS fire DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS sdf DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS bumpmapping DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS explosion DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS pbr DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS projective DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS shadow-mapping DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS shadow-mapping-2 DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS burn DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS burn-2 DESTINATION  ${CMAKE_SOURCE_DIR} )
endif ()