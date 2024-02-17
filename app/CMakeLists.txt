cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

project(rinha VERSION "0.0.1" LANGUAGES CXX)


#find_package(Drogon REQUIRED)

include(cmake/CPM.cmake)
CPMAddPackage(
  NAME Drogon
  VERSION 1.8.4
  GITHUB_REPOSITORY "drogonframework/drogon"
  GIT_TAG "v1.8.4"
)

set(APP_EXECUTABLE_NAME rinha)
add_executable(${APP_EXECUTABLE_NAME} src/main.cpp )
include_directories(include)
target_link_libraries(${APP_EXECUTABLE_NAME} PUBLIC pthread drogon)
install (TARGETS ${APP_EXECUTABLE_NAME})


