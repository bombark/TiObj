# The name of our project is "HELLO". CMakeLists files in this project can 
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and 
# to the root binary directory of the project as ${HELLO_BINARY_DIR}. 
cmake_minimum_required (VERSION 2.6) 
project (TiObj) 



SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -O2")

add_library (tiobj src-cpp/tiobj.cpp)
install(TARGETS tiobj DESTINATION lib)
install(FILES   src-cpp/tiobj.hpp DESTINATION include)
