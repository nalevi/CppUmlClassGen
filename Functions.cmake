# Generate ODB files from sources
# @return ODB_CXX_SOURCES - odb cxx source files
function(generate_odb_files _src)
  foreach(_file ${_src})
    get_filename_component(_dir ${_file} DIRECTORY)
    get_filename_component(_name ${_file} NAME)

    string(REPLACE ".h" "-odb.cxx" _cxx ${_name})
    string(REPLACE ".h" "-odb.hxx" _hxx ${_name})
    string(REPLACE ".h" "-odb.ixx" _ixx ${_name})
    string(REPLACE ".h" "-odb.sql" _sql ${_name})

    add_custom_command(
      OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/${_cxx}
        ${CMAKE_CURRENT_BINARY_DIR}/include/model/${_hxx}
        ${CMAKE_CURRENT_BINARY_DIR}/include/model/${_ixx}
        ${CMAKE_CURRENT_BINARY_DIR}/include/model/${_sql}
      COMMAND
        mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/include/model
      COMMAND
        ${ODB_EXECUTABLE} ${ODBFLAGS}
          -o ${CMAKE_CURRENT_BINARY_DIR}/include/model
          -I ${CMAKE_CURRENT_SOURCE_DIR}/include
          -I ${CMAKE_SOURCE_DIR}/model/include
          -I ${CMAKE_SOURCE_DIR}/util/include
          -I ${ODB_INCLUDE_DIRS}
          ${CMAKE_CURRENT_SOURCE_DIR}/${_file}
      COMMAND
        mv ${CMAKE_CURRENT_BINARY_DIR}/include/model/${_cxx}
           ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/${_file}
      COMMENT "Generating ODB for ${_file}")

    list(APPEND SOURCES ${_cxx})
  endforeach(_file)

  set(ODB_CXX_SOURCES ${SOURCES} PARENT_SCOPE)
endfunction(generate_odb_files)

# Add a new static library target that links against ODB.
function(add_odb_library _name)
  add_library(${_name} STATIC ${ARGN})
  target_compile_options(${_name} PUBLIC -Wno-unknown-pragmas -fPIC)
  target_link_libraries(${_name} ${ODB_LIBRARIES})
  target_include_directories(${_name} PUBLIC
    ${ODB_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/util/include
    ${CMAKE_SOURCE_DIR}/model/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/model/include)
endfunction(add_odb_library)

# This function can be used to install the ODB generated .sql files to a
# specific directory. These files will be used to create database tables before
# the parsing session.
function(install_sql)
  install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/model/
    DESTINATION ${INSTALL_SQL_DIR}
    FILES_MATCHING PATTERN "*.sql"
    PATTERN "CMakeFiles" EXCLUDE)
endfunction(install_sql)

# Prints a coloured, and optionally bold message to the console.
# _colour should be some ANSI colour name, like "blue" or "magenta".
function(fancy_message _str _colour _isBold)
  set(BOLD_TAG "")
  set(COLOUR_TAG "")

  if (_isBold)
    set(BOLD_TAG "--bold")
  endif()

  if (NOT (_colour STREQUAL ""))
    set(COLOUR_TAG "--${_colour}")
  endif()

  execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color ${COLOUR_TAG} ${BOLD_TAG} ${_str})
endfunction(fancy_message)

# Joins a list of elements with a given glue string.
# See: https://stackoverflow.com/questions/7172670/best-shortest-way-to-join-a-list-in-cmake
function(join _values _glue _output)
  string (REGEX REPLACE "([^\\]|^);" "\\1${_glue}" _tmpStr "${_values}")
  string (REGEX REPLACE "[\\](.)" "\\1" _tmpStr "${_tmpStr}") #fixes escaping
  set (${_output} "${_tmpStr}" PARENT_SCOPE)
endfunction(join)
