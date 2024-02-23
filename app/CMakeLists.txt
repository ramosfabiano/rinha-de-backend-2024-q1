cmake_minimum_required(VERSION 3.22)

project(rinha VERSION "0.0.1" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)
find_package(jsoncpp REQUIRED)

set(APP_EXECUTABLE_NAME rinha)
add_executable(${APP_EXECUTABLE_NAME} src/main.cc 
                                      src/controllers/RinhaController.cc 
                                      src/models/Clientes.cc 
                                      src/models/Transacoes.cc)

target_include_directories(${APP_EXECUTABLE_NAME} PRIVATE src/ ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(${APP_EXECUTABLE_NAME} PRIVATE ${DROGON_LIBRARIES})

install (TARGETS ${APP_EXECUTABLE_NAME})

