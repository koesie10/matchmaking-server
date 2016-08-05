include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(testclient_files
	client.cpp
)

add_executable(matchmaking-testclient ${testclient_files})

target_link_libraries(matchmaking-testclient libmatchmaking proto libprotobuf ${Boost_LIBRARIES})

# use static linking (compatibility with protobuf)
foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
   if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)