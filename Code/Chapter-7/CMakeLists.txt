cmake_minimum_required (VERSION 2.6)
project (Chapter-7)

include_directories("../Core" "../Libs/SOIL")

add_library(Core  ../Core/TgaImage.cpp .)

add_executable(test test.cpp)
target_link_libraries(test Core)

add_executable(line line.cpp)
target_link_libraries(line Core)

add_executable(circle circle.cpp)
target_link_libraries(circle Core)

add_executable(floodfill floodfill.cpp)
target_link_libraries(floodfill Core)

add_executable(tri tri.cpp)
target_link_libraries(tri Core)

if (WIN32)
	install(TARGETS test DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS line DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS circle DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS floodfill DESTINATION  ${CMAKE_SOURCE_DIR} )
	install(TARGETS tri DESTINATION  ${CMAKE_SOURCE_DIR} )
endif ()
