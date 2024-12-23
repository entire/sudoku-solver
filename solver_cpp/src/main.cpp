#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "solver/sudoku.h"
#include "recognizer/recognizer.h"

void StartVideo();

int main() {

    // recognizer
    Sudoku::Recognizer recognizer;
    recognizer.SetDebugMode(true);
    recognizer.Setup();

    // digit classifier

    // main solver
//    std::vector<int> grid = { 4, 0, 0, 0, 0, 0, 8, 0, 5, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 8, 0, 4, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 6, 0, 3, 0, 7, 0, 5, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0 };
//    Sudoku::Solver solver;
//    solver.Solve(grid);

    return 0;
}

void StartVideo() {
    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Error! couldn't open camera" << std::endl;
    }

    // make a window
    cv::Mat frame;

    while(1) {
        double fps = camera.get(cv::CAP_PROP_FPS); 
        bool bSuccess = camera.read(frame);
        if (!bSuccess) {
            std::cout << "this is the end of the video" << std::endl;
            break;
        }

        // add gaussian blur
        cv::Mat frameBlurred;
        cv::GaussianBlur(frame, frameBlurred, cv::Size(3, 3), 0);
        
        std::cout << "Frames per seconds : " << fps << std::endl;
        cv::imshow("tracker", frame);

        // press 'q' to quit
        if (cv::waitKey(10) >= 0) {
            break;
        }
    }
}