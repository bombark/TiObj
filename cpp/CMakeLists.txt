cmake_minimum_required (VERSION 2.6)
project ("libtiobj")
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -O3")


add_library (tiobj SHARED "src/join.cpp" "src/tiasm.cpp" "src/tivar.cpp" "src/tiparser.cpp" "src/tiobj.cpp" "src/tiasm.cpp" "src/tivarpkg.cpp" "src/tibuilder.cpp" )
#add_library (tiobj-static STATIC "src/join.cpp" "src/tiasm.cpp" "src/tivar.cpp" "src/tiparser.cpp" "src/tiobj.cpp" "src/tiasm.cpp" "src/tivarpkg.cpp" "src/tibuilder.cpp" )
#set_target_properties(tiobj-static PROPERTIES OUTPUT_NAME tiobj)




# EXAMPLES


add_executable("ti2json" "examples/ti2json.cpp")
target_link_libraries("ti2json" tiobj)


install(TARGETS "tiobj" DESTINATION lib)
#install(TARGETS "tiobj-static" DESTINATION lib)
install(TARGETS "ti2json"      DESTINATION bin)

install(FILES "include/tiobj.hpp"      DESTINATION include)
install(FILES "include/tiparser.hpp"   DESTINATION include)
install(FILES "include/tiasm.hpp"      DESTINATION include)

