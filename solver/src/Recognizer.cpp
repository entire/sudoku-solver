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
    cv::Mat image;
    image = cv::imread("../../digit_classifier/assets/puzzle.jpg");

    if (!image.data) {
        std::cout << "could not open image" << std::endl;
        return;
    }

    cv::namedWindow("display window", cv::WINDOW_AUTOSIZE);

    // resize image
    cv::Mat resizeImg, gray, blurred, thresh, bitwiseNot;
    cv::resize(image, resizeImg, cv::Size(), 0.75, 0.75);
    cv::cvtColor(resizeImg, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(7, 7), 3);
    cv::adaptiveThreshold(blurred, thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2.0);
    cv::bitwise_not(thresh, bitwiseNot);

    // setup for getting contours & largest contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> largest_contour;

    // get contours first
    cv::findContours(bitwiseNot, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // sort contour areas
    std::sort(contours.begin(), contours.end(),
              [](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {
                  double i = fabs(contourArea(cv::Mat(contour1)));
                  double j = fabs(contourArea(cv::Mat(contour2)));
                  return (i < j);
              });

    // then get the largest contour
    GetLargestContourFromContours(contours, largest_contour);

//    cv::Mat adjusted;
//    FourPointTransform(largest_contour, resizeImg, adjusted);
//    cv::Mat transform_matrix = cv::getPerspectiveTransform(resizeImg, largest_contour);
//    cv::warpPerspective(resizeImg, transform_matrix, resizeImg.size

    // draw contour
    if (_debug) {
        cv::Scalar color(255, 255, 0);
        cv::drawContours(
                resizeImg,
                std::vector<std::vector<cv::Point>>{largest_contour},
                0,
                color,
                2);

        cv::imshow("display window", resizeImg);
        cv::waitKey(0);
    }


}
void Recognizer::FourPointTransform(std::vector<cv::Point>& contour, cv::Mat& original, cv::Mat& adjusted) {
//# Locate points of the documents or object which you want to transform
//    pts1 = np.float32([[0, 260], [640, 260], [0, 400], [640, 400]])
//    pts2 = np.float32([[0, 0], [400, 0], [0, 640], [400, 640]])
//
//# Apply Perspective Transform Algorithm
//    matrix = cv2.getPerspectiveTransform(pts1, pts2)
//    result = cv2.warpPerspective(frame, matrix, (500, 600))
//# Wrap the transformed image
}

void Recognizer::GetLargestContourFromContours(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largest_contour) {
    std::vector<cv::Point> c_approx;
    int largest_area = 0;

    for (auto& c : contours) {
        double perimeter = cv::arcLength(c, true);
        double epsilon = 0.02 * perimeter;
        cv::approxPolyDP(c, c_approx, epsilon, true);

        if (c_approx.size() == 4) {
            double area = cv::contourArea(c);

            if (area > largest_area) {
                largest_area = area;
                largest_contour = c;
            }
        }
    }

}

} // namespace Sudoku