cmake_minimum_required(VERSION 3.6)

# Global properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Project name
project(cppbenchmark)

# Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN "doxygen")
  if(NOT TARGET ${DOXYGEN})
    add_custom_command(OUTPUT "Doxyfile" COMMAND ${DOXYGEN_EXECUTABLE} "Doxyfile" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/documents")
    add_custom_target(${DOXYGEN} DEPENDS "Doxyfile")
    set_target_properties(${DOXYGEN} PROPERTIES FOLDER doxygen)
  endif()
endif()

# CMake module path
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Compiler features
include(SetCompilerFeatures)
include(SetCompilerWarnings)
include(SetPlatformFeatures)
include(SystemInformation)

# External packages
find_package(Threads REQUIRED)

# Modules
add_subdirectory("modules")

# Link libraries
list(APPEND LINKLIBS Threads::Threads)

# System directories
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/modules")

# Library
file(GLOB_RECURSE SOURCE_FILES "source/*.cpp")
set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_FLAGS "${PEDANTIC_COMPILE_FLAGS}")
add_library(cppbenchmark ${SOURCE_FILES})
target_include_directories(cppbenchmark PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(cppbenchmark ${LINKLIBS} cpp-optparse HdrHistogram)
set_target_properties(cppbenchmark PROPERTIES FOLDER libraries)
list(APPEND INSTALL_TARGETS cppbenchmark)
list(APPEND LINKLIBS cppbenchmark)

# Additional module components: benchmarks, examples, plugins, tests, tools and install
if(NOT CPPBENCHMARK_MODULE)

  # Examples
  file(GLOB EXAMPLE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/examples" "examples/*.cpp")
  foreach(EXAMPLE_FILE ${EXAMPLE_FILES})
    string(REGEX REPLACE "(.*)\\.cpp" "\\1" EXAMPLE_NAME ${EXAMPLE_FILE})
    set(EXAMPLE_TARGET "cppbenchmark-example-${EXAMPLE_NAME}")
    set_source_files_properties(examples/${EXAMPLE_FILE} PROPERTIES COMPILE_FLAGS "${PEDANTIC_COMPILE_FLAGS}")
    add_executable(${EXAMPLE_TARGET} examples/${EXAMPLE_FILE})
    target_link_libraries(${EXAMPLE_TARGET} ${LINKLIBS})
    set_target_properties(${EXAMPLE_TARGET} PROPERTIES FOLDER examples)
    list(APPEND INSTALL_TARGETS ${EXAMPLE_TARGET})
    list(APPEND INSTALL_TARGETS_PDB ${EXAMPLE_TARGET})
  endforeach()

  # Tests
  file(GLOB TESTS_SOURCE_FILES "tests/*.cpp")
  set_source_files_properties(${TESTS_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "${PEDANTIC_COMPILE_FLAGS}")
  add_executable(cppbenchmark-tests ${TESTS_SOURCE_FILES})
  target_include_directories(cppbenchmark-tests PRIVATE ${Catch2})
  target_link_libraries(cppbenchmark-tests ${LINKLIBS})
  set_target_properties(cppbenchmark-tests PROPERTIES FOLDER tests)
  list(APPEND INSTALL_TARGETS cppbenchmark-tests)
  list(APPEND INSTALL_TARGETS_PDB cppbenchmark-tests)

  # CTest
  enable_testing()
  add_test(cppbenchmark-tests cppbenchmark-tests --durations yes --order lex)

  # Install
  install(TARGETS ${INSTALL_TARGETS}
    RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/bin"
    LIBRARY DESTINATION "${PROJECT_SOURCE_DIR}/bin"
    ARCHIVE DESTINATION "${PROJECT_SOURCE_DIR}/bin")

  # Install *.pdb files
  if(MSVC)
    foreach(INSTALL_TARGET_PDB ${INSTALL_TARGETS_PDB})
      install(FILES $<TARGET_PDB_FILE:${INSTALL_TARGET_PDB}> DESTINATION "${PROJECT_SOURCE_DIR}/bin")
    endforeach()
  endif()

endif()
