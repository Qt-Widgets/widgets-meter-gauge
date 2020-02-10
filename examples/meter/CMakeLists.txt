
project( meter )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( SRC main.cpp )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../../include )

link_directories( ${CMAKE_CURRENT_BINARY_DIR}/../../lib )

add_executable( meter ${SRC} )

target_link_libraries( meter widgets Qt5::Widgets )
