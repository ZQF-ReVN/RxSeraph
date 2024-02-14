# Project
project(Seraph)

# Set Source File Path
list(APPEND "${PROJECT_NAME}_SRC_FILE" 
	"src/Seraph/Dat.cpp")

# Set Library
add_library("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC_FILE}")
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_23)
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
target_include_directories("${PROJECT_NAME}" INTERFACE "src")

# Add Library
target_link_libraries("${PROJECT_NAME}" PRIVATE Rut zlibstatic)
