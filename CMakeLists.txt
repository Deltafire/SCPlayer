cmake_minimum_required(VERSION 3.13.4)

project (scplayer)

set(Z80_DIR extern/Z80)
set(SAASOUND_DIR extern/SAASound)

set(Z80_SOURCES
  ${Z80_DIR}/Codes.h
  ${Z80_DIR}/CodesCB.h
  ${Z80_DIR}/CodesED.h
  ${Z80_DIR}/CodesXCB.h
  ${Z80_DIR}/CodesXX.h
  ${Z80_DIR}/ConDebug.c
  ${Z80_DIR}/Debug.c
  ${Z80_DIR}/Tables.h
  ${Z80_DIR}/Z80.c
  ${Z80_DIR}/Z80.h
)

add_library(${PROJECT_NAME}
  src/SCPlayer.cpp
  src/etracker_bin.h
  ${Z80_SOURCES}
)
#add_library(sub::libscplayer ALIAS scplayer)

target_include_directories( ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/${Z80_DIR}
)

# Use system SAASound if found, otherwise build our own
find_library(SAASOUND_LIBRARY NAMES SAASound saasound)
find_path(SAASOUND_INCLUDE_DIR SAASound.h)
if (SAASOUND_LIBRARY AND SAASOUND_INCLUDE_DIR)
  target_include_directories(${PROJECT_NAME} PRIVATE ${SAASOUND_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${SAASOUND_LIBRARY})
  message(STATUS "Found SAASound: ${SAASOUND_LIBRARY}")
else()
  message(STATUS "Building SAASound")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${SAASOUND_DIR})
  target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${SAASOUND_DIR}/include)
  target_link_libraries(${PROJECT_NAME} SAASound)
endif()

# Don't build scplay if this is a subproject or SDL isn't found
get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
  find_package(SDL2)
  if(SDL2_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    add_subdirectory(scplay)
  endif()
endif()

# Detect system endianess (needed for Z80.h)
include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(NOT IS_BIG_ENDIAN)
  target_compile_definitions(${PROJECT_NAME} PUBLIC LSB_FIRST)
endif()
