# Project
project(Seraph)

# Set Source File Path
list(APPEND "${PROJECT_NAME}_SRC_FILE" 
	"src/Seraph/Dat.cpp"
	"src/Seraph/Script.cpp"
	"src/Seraph/Script_Reader.cpp"
	"src/Seraph/Script_Scenario.cpp"
	"src/Seraph/Script_Expression.cpp"
	"src/Seraph/Script_Cryptor.cpp")

# Source File Only x32
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    list(APPEND "${PROJECT_NAME}_SRC_FILE" "src/Seraph/VFS.cpp")
endif()

# Set Library
add_library("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC_FILE}")
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_23)
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
target_include_directories("${PROJECT_NAME}" INTERFACE "src")

# Add Library
target_link_libraries("${PROJECT_NAME}" PRIVATE Rut zlibstatic)

# Library Only x32
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
	target_link_libraries("${PROJECT_NAME}" PRIVATE RxHook)
endif()