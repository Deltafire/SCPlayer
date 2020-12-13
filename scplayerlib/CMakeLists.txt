cmake_minimum_required(VERSION 3.18)

project (scplayer)

set(Z80_SOURCES
  Z80/Codes.h
  Z80/CodesCB.h
  Z80/CodesED.h
  Z80/CodesXCB.h
  Z80/CodesXX.h
  Z80/ConDebug.c
  Z80/Debug.c
  Z80/Tables.h
  Z80/Z80.c
  Z80/Z80.h
)

add_library(${PROJECT_NAME}
  src/SCPlayer.cpp
  src/etracker_bin.h
  ${Z80_SOURCES}
)
add_library(sub::libscplayer ALIAS scplayer)

target_include_directories( ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/Z80
)

find_library(SAASOUND_LIBRARY NAMES SAASound saasound)
find_path(SAASOUND_INCLUDE_DIR SAASound.h)
if (SAASOUND_LIBRARY AND SAASOUND_INCLUDE_DIR)
  target_include_directories(${PROJECT_NAME} PRIVATE ${SAASOUND_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${SAASOUND_LIBRARY})
  message(STATUS "Found SAASound: ${SAASOUND_LIBRARY}")
else()
  message(STATUS "Building SAASound")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SAASound)
  target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/SAASound/include)
  target_link_libraries(${PROJECT_NAME} SAASound)
endif()
