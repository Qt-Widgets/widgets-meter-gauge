
project( widgets )

set( SRC meter.hpp
	meter.cpp )
    
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

include_directories( ${Qt5Core_INCLUDE_DIRS}
	${Qt5Gui_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS} )

add_library( widgets STATIC ${SRC} )

set_property( TARGET widgets PROPERTY CXX_STANDARD 14 )

