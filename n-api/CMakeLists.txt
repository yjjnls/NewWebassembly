cmake_minimum_required(VERSION 3.11)
project(lib)
file(GLOB_RECURSE  LWS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch] 
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/../example.cc)
# conan_project_group(LWS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR})

message("-------------------")
message(${CMAKE_CURRENT_SOURCE_DIR}/include/)
message("-------------------")
foreach( filename ${LWS_SOURCES} )
message(${filename})
endforeach()
message("-------------------")

if( MSVC )
	set(libname libwebstreamer)
else()
	set(libname webstreamer)
endif()

# ADD_DEFINITIONS( -Dcxx_rtti=1  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
# include_directories($ENV{EMSCRIPTEN}/system/include)

add_library(${libname}  SHARED ${LWS_SOURCES} )
target_link_libraries(${libname} ${GST_MODULES_LIBRARIES} )


set(instd ${CMAKE_HOME_DIRECTORY}/bin)
if( MSVC )
	set(instd ${instd}/win )
else()
	set(instd ${instd}/linux )
	set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -std=c++11")
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL AMD64 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64 )
	set(instd "${instd}/x64")
else()
	set(instd "${instd}/x86")
endif()
install(TARGETS ${libname} RUNTIME DESTINATION ${instd}
						    LIBRARY DESTINATION ${instd})

