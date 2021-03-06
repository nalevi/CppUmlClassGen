find_library(WT_LIB NAMES wt REQUIRED)
find_library(WT_DBLIB NAMES wtdbo REQUIRED)
find_library(WT_POSTGRES NAMES wtdbopostgres REQUIRED)
find_library(WT_SQLITE3 NAMES wtdbosqlite3 REQUIRED)

message(STATUS "Found Wt lib: " ${WT_LIB})
message(STATUS "Found libwtdbo.so: " ${WT_DBLIB})
message(STATUS "Found libwtdbopostgres.so: " ${WT_POSTGRES})
message(STATUS "Found libwtdbosqlite3.so: " ${WT_SQLITE3})

add_library(model STATIC 
  include/model/cppmethod.h
  include/model/cpprecord.h
  include/model/cppattribute.h
  include/model/visibility.h)

# Wt ORM libraries
target_link_libraries(model 
  wt
  wtdbo
  wtdbopostgres
  wtdbosqlite3)

target_include_directories(model PUBLIC
  ${CMAKE_SOURCE_DIR}/model/include)
set_target_properties(model PROPERTIES LINKER_LANGUAGE CXX)
