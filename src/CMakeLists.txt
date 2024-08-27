# RxSeraph
cmake_minimum_required(VERSION 3.25)

# Project Name
project(ReVN_RxSeraph)

# Export Symbols
if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Set Source File Path
list(APPEND ReVN_RxSeraph_Core_Src_Files
    "src/RxSeraph/Core/Pack.cpp"
    "src/RxSeraph/Core/Pack_Cryptor.cpp"
    "src/RxSeraph/Core/Pack_IndexTable.cpp"
    "src/RxSeraph/Core/Script.cpp"
    "src/RxSeraph/Core/Script_Cryptor.cpp"
    "src/RxSeraph/Core/Script_Expression.cpp"
    "src/RxSeraph/Core/Script_Reader.cpp"
    "src/RxSeraph/Core/Script_Scenario.cpp")

# Set Library
add_library(ReVN_RxSeraph_Core ${ReVN_RxSeraph_Core_Src_Files})
add_library(ReVN::RxSeraph::Core ALIAS ReVN_RxSeraph_Core)
target_include_directories(ReVN_RxSeraph_Core PUBLIC "src")
target_compile_features(ReVN_RxSeraph_Core PRIVATE cxx_std_23)
target_compile_options(ReVN_RxSeraph_Core PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_link_libraries(ReVN_RxSeraph_Core PRIVATE ZLIB::ZLIB)
target_link_libraries(ReVN_RxSeraph_Core PUBLIC Zut::ZxMem Zut::ZxJson Zut::ZxCvt Zut::ZxFile Zut::ZxFS)

# Warning
if(MSVC)
    target_compile_options(ReVN_RxSeraph_Core PRIVATE /W4)
else()
    target_compile_options(ReVN_RxSeraph_Core PRIVATE -Wall -Wextra)
endif()


# Hook
if((MSVC) AND (CMAKE_SIZEOF_VOID_P EQUAL 4))
    # Set Source File Path
    list(APPEND ReVN_RxSeraph_Hook_Src_Files
        "src/RxSeraph/Hook/VFS.cpp")

    # Set Library
    add_library(ReVN_RxSeraph_Hook ${ReVN_RxSeraph_Hook_Src_Files})
    add_library(ReVN::RxSeraph::Hook ALIAS ReVN_RxSeraph_Hook)
    target_include_directories(ReVN_RxSeraph_Hook PUBLIC "src")
    target_compile_options(ReVN_RxSeraph_Hook PRIVATE /W3)
    target_compile_features(ReVN_RxSeraph_Hook PRIVATE cxx_std_23)
    target_compile_options(ReVN_RxSeraph_Hook PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    target_link_libraries(ReVN_RxSeraph_Hook PRIVATE Zut::ZxHook)
endif()
