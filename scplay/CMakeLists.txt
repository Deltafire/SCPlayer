project(scplay)

add_executable(${PROJECT_NAME} scplay.cpp)

target_link_libraries(${PROJECT_NAME}
  sub::libscplayer
)

find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
