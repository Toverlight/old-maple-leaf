cmake_minimum_required(VERSION 3.20)

get_filename_component(DEFAULT_PROJECT_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
project(${DEFAULT_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		$<$<CONFIG:Debug>:DEBUG>
)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE
		ProjectCommon 
		glad 
        glfw 
		OpenGL::GL 
        glm)