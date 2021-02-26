include_directories(util PUBLIC
  ${CMAKE_SOURCE_DIR}/util/include
  ${CMAKE_SOURCE_DIR}/model/include
  ${BOOST_INCLUDE_DIRS})

add_library(util STATIC
  src/diagramm.cpp)

target_link_libraries(util
  ${Boost_LIBRARIES})

