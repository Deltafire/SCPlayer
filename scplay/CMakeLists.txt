project(scplay)

add_executable(${PROJECT_NAME} scplay.cpp)

target_link_libraries(${PROJECT_NAME}
  scplayer
)
