cmake_minimum_required(VERSION 3.10)
project(NEM)  # 여기서 XX는 실제 프로젝트 이름을 의미합니다

# C++11 표준 사용
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenMP 찾기
find_package(OpenMP REQUIRED)

# 소스 파일 목록
set(SOURCES
    ${CMAKE_SOURCE_DIR}/../src/main.cpp
    ${CMAKE_SOURCE_DIR}/../src/solver.cpp
    ${CMAKE_SOURCE_DIR}/../src/structure.cpp
    ${CMAKE_SOURCE_DIR}/../src/node.cpp
    ${CMAKE_SOURCE_DIR}/../src/cx.cpp
)

# 헤더 파일 경로 추가
include_directories(${CMAKE_SOURCE_DIR}/../src)

# 실행 파일 생성
add_executable(${PROJECT_NAME} ${SOURCES})

# OpenMP 링크
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()