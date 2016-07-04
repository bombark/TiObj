cmake_minimum_required (VERSION 2.6)
project ("libtiobj")
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -O3")


add_library (tiobj SHARED "src/join.cpp" "src/tiasm.cpp" "src/tivar.cpp" "src/tiparser.cpp" "src/tiobj.cpp" "src/tiasm.cpp" "src/tivarpkg.cpp" "src/tibuilder.cpp" )
#add_library (tiobj-static STATIC "src/join.cpp" "src/tiasm.cpp" "src/tivar.cpp" "src/tiparser.cpp" "src/tiobj.cpp" "src/tiasm.cpp" "src/tivarpkg.cpp" "src/tibuilder.cpp" )
#set_target_properties(tiobj-static PROPERTIES OUTPUT_NAME tiobj)


find_package(OpenCV REQUIRED)

add_executable(test_asm "tests/api.cpp")
target_link_libraries(test_asm tiobj  ${OpenCV_LIBS})

add_executable(test_parser "tests/test_parser.cpp")
target_link_libraries(test_parser tiobj)

add_executable(test_speed "tests/test_speed.cpp")
target_link_libraries(test_speed tiobj pthread jansson ${OpenCV_LIBS} yaml-cpp)


add_executable(test_cv "tests/test_cv.cpp")
target_link_libraries(test_cv tiobj ${OpenCV_LIBS})

add_executable(test_basic "tests/basic.cpp")
target_link_libraries(test_basic tiobj ${OpenCV_LIBS})

add_executable(outros "tests/curiosidades.cpp")
target_link_libraries(outros tiobj ${OpenCV_LIBS})



# EXAMPLES

add_executable("stream_filter" "examples/stream_filter.cpp")
target_link_libraries("stream_filter" tiobj)

add_executable("foreach" "examples/foreach.cpp")
target_link_libraries("foreach" tiobj)

add_executable("load" "examples/load.cpp")
target_link_libraries("load" tiobj)

add_executable("save" "examples/save.cpp")
target_link_libraries("save" tiobj)

add_executable("select" "examples/select.cpp")
target_link_libraries("select" tiobj)

add_executable("orderby" "examples/orderby.cpp")
target_link_libraries("orderby" tiobj)

add_executable("groupby" "examples/groupby.cpp")
target_link_libraries("groupby" tiobj)

install(TARGETS "tiobj" DESTINATION lib)
#install(TARGETS "tiobj-static" DESTINATION lib)

install(FILES "include/tiobj.hpp"      DESTINATION include)
install(FILES "include/tiparser.hpp"   DESTINATION include)
install(FILES "include/tiasm.hpp"      DESTINATION include)
install(FILES "include/tiobj-cv.hpp"   DESTINATION include)
