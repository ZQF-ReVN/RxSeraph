# RxSeraph
cmake_minimum_required(VERSION 3.25)

# Project Name
project(Rvn_RxSeraph)

# Export Symbols
if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Set Source File Path
list(APPEND Rvn_RxSeraph_Core_Src_Files
    "Core/RxSeraph/Pack.cpp"
    "Core/RxSeraph/Pack_Cryptor.cpp"
    "Core/RxSeraph/Pack_IndexTable.cpp"
    "Core/RxSeraph/Script.cpp"
    "Core/RxSeraph/Script_Cryptor.cpp"
    "Core/RxSeraph/Script_Expression.cpp"
    "Core/RxSeraph/Script_Reader.cpp"
    "Core/RxSeraph/Script_Scenario.cpp")

# Set Library
add_library(Rvn_RxSeraph_Core ${Rvn_RxSeraph_Core_Src_Files})
add_library(Rvn::RxSeraph::Core ALIAS Rvn_RxSeraph_Core)
target_include_directories(Rvn_RxSeraph_Core PUBLIC "Core")
target_compile_features(Rvn_RxSeraph_Core PRIVATE cxx_std_23)
target_compile_options(Rvn_RxSeraph_Core PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_link_libraries(Rvn_RxSeraph_Core PRIVATE ZLIB::ZLIB)
target_link_libraries(Rvn_RxSeraph_Core PUBLIC Zut::ZxMem Zut::ZxJson Zut::ZxCvt Zut::ZxFile Zut::ZxFS)

# Warning
if(MSVC)
    target_compile_options(Rvn_RxSeraph_Core PRIVATE /W4)
else()
    target_compile_options(Rvn_RxSeraph_Core PRIVATE -Wall -Wextra)
endif()


# Hook
if((MSVC) AND (CMAKE_SIZEOF_VOID_P EQUAL 4))
    # Set Source File Path
    list(APPEND Rvn_RxSeraph_Hook_Src_Files
        "Hook/RxSeraph/VFS.cpp")

    # Set Library
    add_library(Rvn_RxSeraph_Hook ${Rvn_RxSeraph_Hook_Src_Files})
    add_library(Rvn::RxSeraph::Hook ALIAS Rvn_RxSeraph_Hook)
    target_include_directories(Rvn_RxSeraph_Hook PUBLIC "Hook")
    target_compile_options(Rvn_RxSeraph_Hook PRIVATE /W3)
    target_compile_features(Rvn_RxSeraph_Hook PRIVATE cxx_std_23)
    target_compile_options(Rvn_RxSeraph_Hook PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    target_link_libraries(Rvn_RxSeraph_Hook PRIVATE Zut::ZxHook)
endif()
