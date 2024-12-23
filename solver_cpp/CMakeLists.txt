cmake_minimum_required(VERSION 3.14.0)
set(PROJECT_NAME sudoku)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

include_directories(include)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/sudoku.cpp
        src/variables.cpp
        )

# googletest
enable_testing()
find_package(GTest REQUIRED)
add_executable(sudoku_tests 
        test/sudoku_test.cpp
        )
target_link_libraries(sudoku_tests GTest::GTest GTest::Main)