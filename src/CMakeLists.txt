set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

add_definitions( -DBOOST_ALL_NO_LIB )

find_package(Boost 1.61 REQUIRED COMPONENTS system program_options)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(matchmaking_files
	bytehelper.cpp
	protocolhelper.cpp
	protocolhelper.h
)

add_library(libmatchmaking STATIC ${matchmaking_files})

target_link_libraries(libmatchmaking proto libprotobuf ${Boost_LIBRARIES})

add_subdirectory(testclient)
add_subdirectory(server)

# use static linking (compatibility with protobuf)
foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
   if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)