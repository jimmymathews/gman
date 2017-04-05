CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(gman)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(include/)
INCLUDE_DIRECTORIES(key_registration_module/include)
INCLUDE_DIRECTORIES(third_party_modules/pugixml)

SET(sources
third_party_modules/pugixml/pugixml.cpp
source/data/database_manager.cpp
source/gui/screen_handler.cpp
source/key_handling/key_handler.cpp
source/gui/category_bar.cpp
source/gui/category_tile.cpp
source/data_interaction/tree_viewer.cpp
source/Main.cpp
)

ADD_EXECUTABLE(gman ${sources})
TARGET_LINK_LIBRARIES(gman ncursesw)
TARGET_LINK_LIBRARIES(gman boost_regex)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

