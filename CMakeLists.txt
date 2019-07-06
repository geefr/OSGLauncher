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

# TODO: If I wasn't so lazy here could have a single setup
# On windows we're using the latest tinyxml, which includes cmake integration
# On non-windows we're using the package manager's version, which for current
# build config doesn't.
# So for now it's a bunch of hacks here
if( MSVC )
  find_package( tinyxml2 REQUIRED )
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

target_link_libraries( ${PROJECT_NAME} ${OPENSCENEGRAPH_LIBRARIES} )
if( MSVC )
target_link_libraries( ${PROJECT_NAME} tinyxml2::tinyxml2 )
endif()

install( TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION bin )
install( DIRECTORY ${PROJECT_SOURCE_DIR}/config
         DESTINATION "config" )
if( MSVC)
# Because windows
  install( FILES ${OPENSCENEGRAPH_LIBRARIES} DESTINATION bin )
  install( FILES ${TINYXML2_LIBRARIES} DESTINATION bin )
endif()
