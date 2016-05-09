cmake_minimum_required (VERSION 2.6)
project ("libtiobj")
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS "-std=c++11 -O2")


add_library (tiobj SHARED "src/join.cpp" "src/tiasm.cpp" "src/tistream.cpp" "src/tivar.cpp" "src/tiparser.cpp" "src/tiobj.cpp" "src/tiasm.cpp" "src/tivet.cpp" "src/tibuilder.cpp" )
add_library (tiobj-static STATIC "src/join.cpp" "src/tiasm.cpp" "src/tistream.cpp" "src/tivar.cpp" "src/tiparser.cpp" "src/tiobj.cpp" "src/tiasm.cpp" "src/tivet.cpp" "src/tibuilder.cpp" )
set_target_properties(tiobj-static PROPERTIES OUTPUT_NAME tiobj)


add_executable(test_asm "tests/api.cpp")
target_link_libraries(test_asm tiobj)


install(TARGETS "tiobj" DESTINATION lib)
install(TARGETS "tiobj-static" DESTINATION lib)

install(FILES "include/tiobj.hpp"    DESTINATION include)
install(FILES "include/tiparser.hpp" DESTINATION include)
install(FILES "include/tiasm.hpp"    DESTINATION include)

