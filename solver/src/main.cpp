#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "solver/sudoku.h"
#include "recognizer/recognizer.hpp"
#include "recognizer/inference.hpp"

void PrintGrid(const std::vector<int>& grid) {
    std::cout << "┌───────┬───────┬───────┐" << std::endl;
    for (int i = 0; i < 9; i++) {
        std::cout << "│ ";
        for (int j = 0; j < 9; j++) {
            if (grid[i * 9 + j] == 0) {
                std::cout << ". ";
            } else {
                std::cout << grid[i * 9 + j] << " ";
            }
            if ((j + 1) % 3 == 0) std::cout << "│ ";
        }
        std::cout << std::endl;
        if ((i + 1) % 3 == 0 && i < 8) {
            std::cout << "├───────┼───────┼───────┤" << std::endl;
        }
    }
    std::cout << "└───────┴───────┴───────┘" << std::endl;
}

int main(int argc, char** argv) {
    std::string imagePath;
    bool useVideo = false;
    bool debug = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            imagePath = argv[i + 1];
            i++;
        } else if (arg == "-v") {
            useVideo = true;
        } else if (arg == "-d") {
            debug = true;
        }
    }

    try {
        Sudoku::Recognizer recognizer(debug);
        recognizer.Setup();
        
        if (useVideo) {
            cv::VideoCapture cap(0);
            if (!cap.isOpened()) {
                throw std::runtime_error("Error opening video capture");
            }
            
            cv::Mat frame;
            while (cap.read(frame)) {
                try {
                    std::vector<int> grid = recognizer.RecognizeFromFrame(frame);
                    system("clear");  // Clear console (use "cls" on Windows)
                    PrintGrid(grid);
                    
                    if (cv::waitKey(10) >= 0) break;
                } catch (const std::exception& e) {
                    std::cerr << "Frame processing error: " << e.what() << std::endl;
                }
            }
        } else {
            if (imagePath.empty()) {
                throw std::runtime_error("Usage: " + std::string(argv[0]) + 
                                       " -i <image_path> [-d] or -v [-d] for video");
            }
            
            std::vector<int> grid = recognizer.RecognizeFromImage(imagePath);
            PrintGrid(grid);
            
            // Solve the puzzle if valid
            if (std::count(grid.begin(), grid.end(), 0) < 81) {  // At least one digit recognized
                Sudoku::Solver solver;  // Create solver instance
                std::vector<int> solution = solver.Solve(grid);  // Use solver directly
                if (!solution.empty()) {
                    std::cout << "\nSolution:" << std::endl;
                    PrintGrid(solution);
                } else {
                    std::cout << "\nNo valid solution found!" << std::endl;
                }
            } else {
                std::cout << "\nNo digits were recognized!" << std::endl;
            }
            
            if (debug) {
                cv::waitKey(0);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}