include(FetchContent)

FetchContent_Declare(
  ZxFile
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFile.git
  GIT_TAG 79d5ec0f142f1819801c33e1f0410dc31e1ba07c)

FetchContent_MakeAvailable(ZxFile)

FetchContent_Declare(
  ZxMem
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxMem.git
  GIT_TAG 3a53c99ca6524058da4448c9b0148d70bda82036)

FetchContent_MakeAvailable(ZxMem)

FetchContent_Declare(
  ZxCvt
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxCvt.git
  GIT_TAG a38452d98b765daf08c03ec8fb82dc67e2fde3dd)

FetchContent_MakeAvailable(ZxCvt)

FetchContent_Declare(
  ZxJson
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxJson.git
  GIT_TAG db32283633cea96b3799a4eb60c0421183f2ac19)

FetchContent_MakeAvailable(ZxJson)

set(ZLIB_BUILD_EXAMPLES OFF)
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG v1.3.1)

FetchContent_MakeAvailable(zlib)
