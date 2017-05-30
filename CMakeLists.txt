cmake_minimum_required (VERSION 3.4.3)
project (ShadowMapping)

# the version number.
set (ShadowMapping_VERSION_MAJOR 1)
set (ShadowMapping_VERSION_MINOR 3)
set (ShadowMapping_ASSET_DIR "../assets/")

# if Visual Studio
if(MSVC)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GF")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
 
# add paths for include files
include_directories("${PROJECT_SOURCE_DIR}/3DRenderer/include" "${PROJECT_SOURCE_DIR}/3DRenderer/include/freeglut/include")

# set up defines in Config.h the be used within the program
configure_file (
  "${PROJECT_SOURCE_DIR}/3DRenderer/include/Config.h.in"
  "${PROJECT_SOURCE_DIR}/3DRenderer/include/Config.h"
  )

# copy asset directory to build directory
file(COPY 3DRenderer/assets DESTINATION ${CMAKE_BINARY_DIR})

# set source files
file(GLOB FREEGLUT_FILES ${PROJECT_SOURCE_DIR}/3DRenderer/include/freeglut/src/*.c)
file(GLOB SOIL_FILES 
"${PROJECT_SOURCE_DIR}/3DRenderer/include/soil/*.c"
"${PROJECT_SOURCE_DIR}/3DRenderer/include/soil/*.h")

# add the executable
add_executable(ShadowMapping ${PROJECT_SOURCE_DIR}/3DRenderer/src/Main.cpp
                             ${PROJECT_SOURCE_DIR}/3DRenderer/src/Application.cpp
			     ${PROJECT_SOURCE_DIR}/3DRenderer/src/glew.c
			     ${PROJECT_SOURCE_DIR}/3DRenderer/src/ObjObject.cpp
			     ${PROJECT_SOURCE_DIR}/3DRenderer/src/Program.cpp
			     ${PROJECT_SOURCE_DIR}/3DRenderer/src/Shaders.cpp
			     ${PROJECT_SOURCE_DIR}/3DRenderer/src/Texture.cpp
			     ${PROJECT_SOURCE_DIR}/3DRenderer/src/VertexBuffer.cpp
			     ${PROJECT_SOURCE_DIR}/3DRenderer/src/Window.cpp

                             ${PROJECT_SOURCE_DIR}/3DRenderer/include/glew.h
                             ${PROJECT_SOURCE_DIR}/3DRenderer/include/glFunctions.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/glut.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/wglew.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/tiny_obj_loader.h
                             ${PROJECT_SOURCE_DIR}/3DRenderer/include/Application.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/ObjObject.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/Program.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/Texture.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/VertexBuffer.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/Window.h
			     ${PROJECT_SOURCE_DIR}/3DRenderer/include/Config.h
			     ${FREEGLUT_FILES}
			     ${SOIL_FILES})
# set folders in IDE
source_group("FreeGlut" FILES ${FREEGLUT_FILES})
source_group("Soil" FILES ${SOIL_FILES})