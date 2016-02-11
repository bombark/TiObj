# The name of our project is "HELLO". CMakeLists files in this project can 
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and 
# to the root binary directory of the project as ${HELLO_BINARY_DIR}. 
cmake_minimum_required (VERSION 2.6) 
project (TiObj) 




SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -O2")

add_library (tiobj SHARED src/tivar.cpp src/tivet.cpp src/tiobj.cpp src/parser.cpp src/join.cpp src/tiasm.cpp src/titextasm.cpp src/tistream.cpp)


#add_executable        (tiparser      src-bin/tiparser.cpp)
#target_link_libraries (tiparser tiobj)

add_executable        (examples_load examples/load.cpp)
target_link_libraries (examples_load tiobj)

#add_executable        (test examples/teste.cpp)
#target_link_libraries (test tiobj)

install(TARGETS tiobj    DESTINATION lib)
#install(TARGETS tiparser DESTINATION bin)
install(FILES   include/tiobj.hpp       DESTINATION include)
install(FILES   include/tiasm.hpp       DESTINATION include)
install(FILES   include/titextasm.hpp   DESTINATION include)
