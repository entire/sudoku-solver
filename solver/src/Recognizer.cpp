//
// Created by kosuke on 9/16/20.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ocr/Recognizer.h"

namespace Sudoku {

Recognizer::Recognizer() {}

Recognizer::~Recognizer() {}

void Recognizer::Setup() {
    std::cout << "setup" << std::endl;

    cv::Mat image;
    image = cv::imread("../../digit_classifier/assets/puzzle.jpg");

    if (!image.data) {
        std::cout << "could not open image" << std::endl;
        return;
    }

    cv::namedWindow("display window", CV_WINDOW_AUTOSIZE);

    // resize image
    cv::Mat resizeImg, gray, blurred, thresh, bitwiseNot;
    cv::resize(image, resizeImg, cv::Size(), 0.75, 0.75);
    cv::cvtColor(resizeImg, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(7,7), 3);
    cv::adaptiveThreshold(blurred, thresh, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 11, 2.0);
    cv::bitwise_not(thresh, bitwiseNot);

    cv::imshow("display window", bitwiseNot);
    cv::waitKey(0);
}

}