cmake_minimum_required(VERSION 3.10)
project(NEM CXX)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenMP
find_package(OpenMP REQUIRED)

# 소스 파일 목록 (NEM를 소스 루트로 가정)
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/solver.cpp
    ${CMAKE_SOURCE_DIR}/src/structure.cpp
    ${CMAKE_SOURCE_DIR}/src/node.cpp
    ${CMAKE_SOURCE_DIR}/src/cx.cpp
)

# 헤더 경로
include_directories(${CMAKE_SOURCE_DIR}/src)

# 실행 파일
add_executable(${PROJECT_NAME} ${SOURCES})

# OpenMP 링크
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()
