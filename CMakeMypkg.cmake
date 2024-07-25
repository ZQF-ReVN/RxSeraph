include(FetchContent)

FetchContent_Declare(
  ZxFile
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFile.git
  GIT_TAG c47f65febc8dce8c55b61bb8d4ce6ab45659f144)

FetchContent_MakeAvailable(ZxFile)

FetchContent_Declare(
  ZxMem
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxMem.git
  GIT_TAG 3a53c99ca6524058da4448c9b0148d70bda82036)

FetchContent_MakeAvailable(ZxMem)

FetchContent_Declare(
  ZxCvt
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxCvt.git
  GIT_TAG 3c82f10f0913cb5810123c2a52ab7197a103cb4f)

FetchContent_MakeAvailable(ZxCvt)

FetchContent_Declare(
  ZxJson
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxJson.git
  GIT_TAG 46e2de23c6e40ef26b838620637d660c2f076017)

FetchContent_MakeAvailable(ZxJson)

FetchContent_Declare(
  ZxFS
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFS.git
  GIT_TAG b8fa173334e845a9d39c5627865ff6f456d89d4d)

FetchContent_MakeAvailable(ZxFS)

if(WIN32)
    set(ZLIB_BUILD_EXAMPLES OFF)
    FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG v1.3.1)

    FetchContent_MakeAvailable(zlib)

    add_library(ZLIB::ZLIB ALIAS zlibstatic)
elseif(UNIX)
    find_package(ZLIB REQUIRED)
endif()
