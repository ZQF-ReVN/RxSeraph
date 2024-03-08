add_subdirectory("Rut")
add_subdirectory("Seraph")

# Library Only x32
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
	add_subdirectory("RxHook")
endif()