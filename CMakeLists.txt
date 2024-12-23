cmake_minimum_required(VERSION 3.26.0)
set(PROJECT_NAME sudoku)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Add LibTorch
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# Add OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(solver/include)
include_directories(recognizer/include)

add_executable(${PROJECT_NAME}
        solver/src/main.cpp
        solver/src/variables.cpp
        solver/src/sudoku.cpp
        recognizer/src/recognizer.cpp
        recognizer/src/inference.cpp
        recognizer/src/overlayer.cpp
        )
# Link OpenCV and LibTorch
target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES} ${OpenCV_LIBS})

# googletest
enable_testing()
find_package(GTest REQUIRED)
add_executable(sudoku_tests 
        solver/test/sudoku_test.cpp
        )
target_link_libraries(sudoku_tests GTest::GTest GTest::Main)