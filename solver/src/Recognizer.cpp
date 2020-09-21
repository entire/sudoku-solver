//
// Created by kosuke on 9/16/20.
//

#include <iostream>
#include <numeric>
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
    cv::Mat resizeImg, gray, blurred, thresh, bitwiseNot, adjusted;
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

    // then get the four point transformed de-skewed grid
    FourPointTransform(largest_contour, resizeImg, adjusted);

    // get ready to separate out the cell
    std::vector<int> nums {1,2,3,4,5,6,7,8,9};
    double width = (double)adjusted.size().width / nums.size();
    double height = (double)adjusted.size().height / nums.size();
    double start_x = 0.0;
    double start_y = 0.0;

    for (auto& i : nums) {
        for (auto &j : nums) {
            start_x = double(width*(i-1));
            start_y = double(height*(j-1));

            // get just the cell
            cv::Rect cellROI(start_x, start_y, width, height);
            cv::Mat cropped = adjusted(cellROI);
            std::string name = "adjusted" + std::to_string(i) + std::to_string(j);
            cv::Mat thresh_cell, gray_cell, resize_cell;
            cv::cvtColor(cropped, gray_cell, cv::COLOR_BGR2GRAY);
            cv::threshold(gray_cell, thresh_cell, 0,255, cv::THRESH_BINARY_INV+cv::THRESH_OTSU);

            // get rid of border around the image
            int padding = thresh_cell.size().width * 0.12;
            cv::Rect removeBordersROI(padding, padding, thresh_cell.cols-(padding*2), thresh_cell.rows-(padding*2));
            cv::Mat borderless_cell = thresh_cell(removeBordersROI);

            // get individual cell contours
            std::vector<std::vector<cv::Point>> cell_contours;
            cv::findContours(borderless_cell, cell_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            std::cout << cell_contours.size() << std::endl;
            cv::imshow(name, borderless_cell);

            int max_area = 0;
            int cell_contour_index = 0;

            for (int j = 0; j < cell_contours.size(); j++) {
                double new_area = cv::contourArea(cell_contours[j]);
                if (new_area > max_area) {
                    max_area = new_area;
                    cell_contour_index = j;
                }
            }

        }
    }

   cv::imshow("adjusted", adjusted);

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
    std::sort(contour.begin(), contour.end(),
              [](cv::Point p1, cv::Point p2) {
                  if (p1.x < p2.x) { return (p1.x < p2.x); }
                  if (p1.x > p2.x) { return (p1.x > p2.x); }
              });

    // Assemble a rotated rectangle out of that info
    cv::RotatedRect box = minAreaRect(cv::Mat(contour));
    cv::Point2f pts[4];
    box.points(pts);

    cv::Point2f src_vertices[4];
    src_vertices[0] = contour[0];
    src_vertices[1] = contour[1];
    src_vertices[2] = contour[2];
    src_vertices[3] = contour[3];

    // just use width of box side for all sides so that warped mat turns into a square
    float boxSide = box.boundingRect2f().width;

    cv::Point2f dst_vertices[4];
    // bottom left
    dst_vertices[0] = cv::Point(boxSide, boxSide);
    // bottom right
    dst_vertices[1] = cv::Point(0, boxSide);
    // top left
    dst_vertices[2] = cv::Point(0, 0);
    // top right
    dst_vertices[3] = cv::Point(boxSide, 0);

    cv::Mat warp;
    warp = cv::getPerspectiveTransform(src_vertices, dst_vertices);
    cv::warpPerspective(original, adjusted, warp, cv::Size(boxSide, boxSide), cv::INTER_LINEAR, cv::BORDER_CONSTANT);
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
                largest_contour = c_approx;
            }
        }
    }
}

} // namespace Sudoku