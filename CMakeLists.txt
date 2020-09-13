cmake_minimum_required(VERSION 3.14.0)
set(PROJECT_NAME sudoku)
project(${PROJECT_NAME})
find_package( OpenCV REQUIRED )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

include_directories(include)
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Sudoku.cpp
        src/Metadata.cpp
        )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

enable_testing()
find_package(GTest REQUIRED)
add_executable(sudoku_tests 
        test/SudokuTest.cpp
        )
target_link_libraries(sudoku_tests GTest::GTest GTest::Main)