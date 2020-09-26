cmake_minimum_required(VERSION 3.14.0)
set(PROJECT_NAME sudoku)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

include_directories(solver/include)
include_directories(digit_recognizer/include)

add_executable(${PROJECT_NAME}
        solver/src/main.cpp
        solver/src/Sudoku.cpp
        solver/src/Variables.cpp
        digit_recognizer/src/Recognizer.cpp
        )

        # pytorch
find_package(Torch REQUIRED)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

# opencv
find_package( OpenCV REQUIRED )
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${TORCH_LIBRARIES})

# googletest
enable_testing()
find_package(GTest REQUIRED)
add_executable(sudoku_tests 
        solver/test/SudokuTest.cpp
        )
target_link_libraries(sudoku_tests GTest::GTest GTest::Main)