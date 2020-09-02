#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <sudoku/Sudoku.h>

void startVideo();
void testMatrix();

int main() {
    // startVideo();
    testMatrix();
    return 0;
}

void testMatrix() {
    std::vector<std::vector<int>> matrix { 
        { 0, 9, 0, 0, 0, 0, 8, 5, 3 },
	    { 0, 0, 0, 8, 0, 0, 0, 0, 4 },
	    { 0, 0, 8, 2, 0, 3, 0, 6, 9 },
	    { 5, 7, 4, 0, 0, 2, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 9, 0, 0, 6, 3, 7 },
	    { 9, 4, 0, 1, 0, 8, 5, 0, 0 },
	    { 7, 0, 0, 0, 0, 6, 0, 0, 0 },
	    { 6, 8, 2, 0, 0, 0, 0, 9, 0 } 
    };
    Sudoku::Solver solver;
    // solver.printMatrix(matrix);
    solver.solve();
}

void startVideo() {
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