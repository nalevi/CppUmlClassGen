# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

link_directories(${LLVM_LIBRARY_DIRS})

# Find Clang
find_package(Clang REQUIRED CONFIG)
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

find_library(CLANG_LIBRARY NAMES clang-cpp REQUIRED HINTS ${LLVM_LIBRARY_DIRS})
message(STATUS "Found clang_library ${CLANG_LIBRARY}")
if(${CLANG_LIBRARY} STREQUAL "CLANG_LIBRARY-NOTFOUND")
  set(NEED_LIBCLANG_COMPONENT_LIBS on)
endif()

if(NEED_LIBCLANG_COMPONENT_LIBS)
  set(CLANG_LIBRARY
    clangTooling
    clangFrontendTool
    clangFrontend
    clangDriver
    clangSerialization
    clangCodeGen
    clangParse
    clangSema
    clangStaticAnalyzerFrontend
    clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore
    clangAnalysis
    clangARCMigrate
    clangRewrite
    clangRewriteFrontend
    clangEdit
    clangAST
    clangASTMatchers
    clangLex
    clangBasic
    LLVMFrontendOpenMP)
endif()
# Only search for clang if we build out of tree
link_directories(${LLVM_LIB_PATH} ${LLVM_LIBRARY_DIRS})
link_directories(${CLANG_LIB_PATH})

add_definitions(${LLVM_DEFINITIONS})

find_library(LLVM_LIBRARY NAMES LLVM PATHS ${LLVM_LIBRARY_DIRS} NO_DEFAULT_PATH)
if(NOT ${LLVM_LIBRARY} STREQUAL "LLVM_LIBRARY-NOTFOUND")
  message(STATUS "Found consolidated shared LLVM lib " ${LLVM_LIBRARY} " that will be linked against.")
  set(USE_LLVM_FAT_LIB on)
endif()

#find_library(WT_LIB NAMES wt REQUIRED)
#message(STATUS "Found Wt: " ${WT_LIB})

include_directories(
   include
   ${PROJECT_SOURCE_DIR}/model/include)

include_directories(SYSTEM
  ${LLVM_INCLUDE_DIRS}
  ${CLANG_INCLUDE_DIRS})

add_executable(generator 
  src/generator.cpp
  src/dbsession.cpp)

if(USE_LLVM_FAT_LIB)
  llvm_config(generator USE_SHARED ${LLVM_LINK_COMPONENTS})
endif()

target_link_libraries(generator
  LINK_PUBLIC
  ${CLANG_LIBRARY}
  ${WT_LIB}
  model)
