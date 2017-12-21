cmake_minimum_required(VERSION 2.8)

project(OSGLauncher)

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_FLAGS "-Werror=all" )

find_package( OpenSceneGraph )
if( NOT OPENSCENEGRAPH_FOUND )
  message( FATAL_ERROR "OpenSceneGraph is required to build the applications" )
endif()

find_package(PkgConfig REQUIRED)
pkg_search_module(TINYXML2 REQUIRED tinyxml2)

######################################
set( SRCS
  main.cpp
  menuentry.cpp
  inputhandler.cpp )

add_executable( ${PROJECT_NAME} ${SRCS} )

target_include_directories( ${PROJECT_NAME} PUBLIC ${TINYXML2_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${TINYXML2_LIBRARIES} )
target_compile_options( ${PROJECT_NAME} PUBLIC ${TINYXML2_CFLAGS_OTHER} )

target_link_libraries( ${PROJECT_NAME}
  osg
  osgDB
  osgGA
  osgViewer )

install( TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION bin )
install( DIRECTORY ${PROJECT_SOURCE_DIR}/config
         DESTINATION "config" )
