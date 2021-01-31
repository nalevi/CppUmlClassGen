find_library(WT_LIB NAMES wt REQUIRED)

link_directories(${WT_LIB})
message(STATUS "Found Wt lib: " ${WT_LIB})
