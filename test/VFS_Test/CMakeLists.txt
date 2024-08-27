# x32 Only
if ((NOT CMAKE_SIZEOF_VOID_P EQUAL 4) OR (NOT MSVC))
	return()
endif()

# Project Name
project(ReVN_RxSeraph_VFS_Test)

# Test Project
add_library("${PROJECT_NAME}" MODULE "dllmain.cpp")
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_23)
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_link_libraries("${PROJECT_NAME}" PRIVATE ReVN::RxSeraph::Hook Zut::ZxHook)
target_compile_options("${PROJECT_NAME}" PRIVATE /W3)
