cmake_minimum_required(VERSION 2.8)

project(OSGLauncher)

set( CMAKE_CXX_STANDARD 11 )

if(MSVC)

else()
    add_compile_options("-Wall" "-Wextra" "-Werror")
endif()

find_package( OpenSceneGraph
    REQUIRED
    osgGA
    osgViewer
    osgText
    osgDB )

if( MSVC )
    if( NOT TINYXML2_INCLUDE_DIRS OR NOT TINYXML2_LIBRARIES)
        message( FATAL_ERROR "TinyXML is required to build")
    endif()
else()
  find_package(PkgConfig REQUIRED)
  pkg_search_module(TINYXML2 REQUIRED tinyxml2)
endif()

######################################
set( SRCS
  main.cpp
  menuentry.cpp
  inputhandler.cpp
  settings.cpp )

add_executable( ${PROJECT_NAME} ${SRCS} )

target_include_directories( ${PROJECT_NAME} PUBLIC ${TINYXML2_INCLUDE_DIRS} ${OSG_INCLUDE_DIR} )
target_link_libraries( ${PROJECT_NAME} ${TINYXML2_LIBRARIES} )
target_compile_options( ${PROJECT_NAME} PUBLIC ${TINYXML2_CFLAGS_OTHER} )

target_link_libraries( ${PROJECT_NAME}
  ${OPENSCENEGRAPH_LIBRARIES} )

install( TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION bin )
install( DIRECTORY ${PROJECT_SOURCE_DIR}/config
         DESTINATION "config" )
if( MSVC)
# Because windows
  install( FILES ${OPENSCENEGRAPH_LIBRARIES} DESTINATION bin )
  install( FILES ${TINYXML2_LIBRARIES} DESTINATION bin )
endif()
