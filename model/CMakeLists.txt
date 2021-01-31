find_library(WT_LIB NAMES wt REQUIRED)

link_directories(${WT_LIB})
message(STATUS "Found Wt lib: " ${WT_LIB})

add_library(model STATIC 
  include/model/cppmethod.h
  include/model/cpprecord.h
  include/model/cppattribute.h
  include/model/visibility.h)

target_link_libraries(model ${WT_LIB})
target_include_directories(model PUBLIC
  ${CMAKE_SOURCE_DIR}/model/include)
set_target_properties(model PROPERTIES LINKER_LANGUAGE CXX)
