cmake_minimum_required(VERSION 3.10)

project(MapleLeaf)
set(CMAKE_CXX_STANDARD 20)
add_definitions(-DDEBUG)

add_library(ProjectCommon INTERFACE)
target_include_directories(ProjectCommon
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw/include
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/dep/glad/include
)

find_package(OpenGL REQUIRED)

add_library(glad STATIC IMPORTED)
set_target_properties(glad PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/dep/glad/lib/glad.lib
)
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw/lib-vc2022/glfw3.lib
)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/dep/glm)

file(GLOB_RECURSE SRC_LIST 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(MapleLeafMain ${SRC_LIST})

target_link_libraries(MapleLeafMain 
	PRIVATE
		ProjectCommon 
		glad 
        glfw 
		OpenGL::GL 
        glm)