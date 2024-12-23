//
// Created by kosuke on 9/16/20.
//

#include <iostream>
#include <numeric>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "recognizer/recognizer.hpp"
#include "recognizer/inference.hpp"
#include "recognizer/overlayer.hpp"
#include "solver/sudoku.h"

namespace Sudoku {

bool IsValidMove(const std::vector<int>& grid, int pos, int num) {
    int row = pos / 9;
    int col = pos % 9;
    
    // Check row
    for (int x = 0; x < 9; x++)
        if (grid[row * 9 + x] == num) return false;
    
    // Check column
    for (int x = 0; x < 9; x++)
        if (grid[x * 9 + col] == num) return false;
    
    // Check 3x3 box
    int box_row = row - row % 3;
    int box_col = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[(box_row + i) * 9 + (box_col + j)] == num) return false;
    
    return true;
}

bool SolveSudokuHelper(std::vector<int>& grid, int pos = 0) {
    if (pos == 81) return true;
    if (grid[pos] != 0) return SolveSudokuHelper(grid, pos + 1);
    
    for (int num = 1; num <= 9; num++) {
        if (IsValidMove(grid, pos, num)) {
            grid[pos] = num;
            if (SolveSudokuHelper(grid, pos + 1)) return true;
            grid[pos] = 0;
        }
    }
    return false;
}

std::vector<int> SolveSudoku(const std::vector<int>& input_grid) {
    std::vector<int> solution = input_grid;
    SolveSudokuHelper(solution);
    return solution;
}

Recognizer::Recognizer() {
    _grid.reserve(81); // Pre-allocate space for 9x9 grid
}

Recognizer::~Recognizer() {}

void Recognizer::Setup() {
    std::cout << "Starting recognition..." << std::endl;
    cv::Mat image = cv::imread("../recognizer/assets/sudoku_3.jpg");
    if (!image.data) {
        throw std::runtime_error("Could not open image file");
    }
    
    // Add validation for grid dimensions
    if (image.rows < 100 || image.cols < 100) {
        throw std::runtime_error("Image dimensions too small for reliable recognition");
    }
    
    std::string filename = "adjusted13_";

    cv::namedWindow("display window", cv::WINDOW_AUTOSIZE);

    // resize image
    cv::Mat resizeImg, gray, blurred, thresh, bitwiseNot, adjusted;
    cv::resize(image, resizeImg, cv::Size(), 0.75, 0.75);

    // then make it into grayscale
    cv::cvtColor(resizeImg, gray, cv::COLOR_BGR2GRAY);

    // then add a gaussian blur
    cv::GaussianBlur(gray, blurred, cv::Size(7, 7), 3);

    // then do som ebinary adaptive thresholding
    cv::adaptiveThreshold(blurred, thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2.0);
    // then get the bitwise not
    cv::bitwise_not(thresh, bitwiseNot);

    // setup for getting contours & largest contours
    std::vector<std::vector<cv::Point>> contours;
    // this is going to be our number that we are loooking for in the cell
    // if this returns empty its size is going to be 0
    std::vector<cv::Point> largest_contour;

    // start with finding all the contours within the Mat
    cv::findContours(bitwiseNot, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // sort contour areas by contour area size
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

    // you need to cast to double here because the adjusted.size().width and height are a double
    // if you return an int, the ROI will be a bit smaller, and inaccurate
    double width = (double)adjusted.size().width / nums.size();
    double height = (double)adjusted.size().height / nums.size();
    double start_x = 0.0;
    double start_y = 0.0;    

    // setup inference with debug flag
    Inference inference(_debug);  // Pass debug flag to constructor
    inference.Setup();

    // Reset grid before starting new recognition
    _grid.clear();
    _grid.reserve(81);

    for (auto& i : nums) {
        for (auto &j : nums) {

            // the starting x and y for the cell
            start_x = double(width*(i-1));
            start_y = double(height*(j-1));

            // get just the cell's region of interest
            cv::Rect cellROI(start_x, start_y, width, height);
            
            // crop it
            cv::Mat cropped = adjusted(cellROI);
            
            // lets get a name that is unique while we loop over so the image will show properly during imshow
            std::string name = filename + std::to_string(i) + std::to_string(j);
            
            // setup mats that we'll need
            cv::Mat thresh_cell, gray_cell, resize_cell;
            
            // make it into grascale 
            cv::cvtColor(cropped, gray_cell, cv::COLOR_BGR2GRAY);
            
            // put an otsu ninarization threshold on it (https://docs.opencv.org/master/d7/d4d/tutorial_py_thresholding.html)
            cv::threshold(gray_cell, thresh_cell, 0,255, cv::THRESH_BINARY_INV+cv::THRESH_OTSU);

            // then we're gonna setup the padding of how much of the border we want to get rid of
            int padding = thresh_cell.size().width * 0.12;
            
            // then remove the border
            cv::Rect removeBordersROI(padding, padding, thresh_cell.cols-(padding*2), thresh_cell.rows-(padding*2));
            cv::Mat borderless_cell = thresh_cell(removeBordersROI);
            cv::Mat last_resize;
            cv::resize(borderless_cell, last_resize, cv::Size(28,28));

            // get individual cell contours
            std::vector<std::vector<cv::Point>> cell_contours;
            cv::findContours(last_resize, cell_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            // Only accept predictions if there's a significant digit contour
            double cell_area = cv::countNonZero(last_resize);
            double total_area = last_resize.rows * last_resize.cols;
            double fill_ratio = cell_area / total_area;

            // If the cell is mostly empty (less than 5% filled), consider it empty
            if (fill_ratio < 0.05) {
                _grid.push_back(0);
                if (_debug) {
                    std::cout << "Position (" << i << "," << j << "): Empty cell" << std::endl;
                }
                continue;
            }

            // Get prediction and add confidence threshold
            int predicted_digit = inference.GetOutputFromImage(last_resize);
            
            // If prediction seems unreliable, mark as empty
            if (predicted_digit < 1 || predicted_digit > 9) {
                predicted_digit = 0;
            }
            
            _grid.push_back(predicted_digit);

            if (_debug) {
                std::cout << "Position (" << i << "," << j << "): " << predicted_digit 
                         << " (fill ratio: " << fill_ratio << ")" << std::endl;
            }
        }
    }

    std::cout << "Grid size after recognition: " << _grid.size() << std::endl;

    // draw contour
    if (_debug) {
        std::cout << "line 151" << std::endl;
        cv::imshow("adjusted", adjusted);

        cv::Scalar color(255, 255, 0);
        cv::drawContours(
                resizeImg,
                std::vector<std::vector<cv::Point>>{largest_contour},
                0,
                color,
                2);
        std::cout << "line 162" << std::endl;
        cv::imshow("display window", resizeImg);
        cv::waitKey(0);
    }
}

void Recognizer::FourPointTransform(std::vector<cv::Point>& contour, cv::Mat& original, cv::Mat& adjusted) {
    std::sort(contour.begin(), contour.end(),
              [](cv::Point p1, cv::Point p2) {
                  if (p1.x < p2.x) return true;
                  if (p1.x > p2.x) return false;
                  return p1.y < p2.y;  // If x coordinates are equal, compare y coordinates
              });

    // assemble a rotated rectangle out of that info
    cv::RotatedRect box = minAreaRect(cv::Mat(contour));
    cv::Point2f pts[4];
    box.points(pts);

    // get all 4  points of the starting verticies
    // remember: this order is important!!
    cv::Point2f src_matrix[4];
    src_matrix[0] = contour[0];
    src_matrix[1] = contour[1];
    src_matrix[2] = contour[2];
    src_matrix[3] = contour[3];

    // just use width of box side for all sides so that warped mat turns into a square
    float boxSide = box.boundingRect2f().width;

    // setup the destination matrix
    cv::Point2f dst_matrix[4];
    // bottom left
    dst_matrix[0] = cv::Point(boxSide, boxSide);
    // bottom right
    dst_matrix[1] = cv::Point(0, boxSide);
    // top left
    dst_matrix[2] = cv::Point(0, 0);
    // top right
    dst_matrix[3] = cv::Point(boxSide, 0);

    // setup transform matrix for doing the linear transformation 
    cv::Mat transformMatrix;
    // to go from the source matrix -> the destination matrix
    transformMatrix = cv::getPerspectiveTransform(src_matrix, dst_matrix);
    // apply the transform
    cv::warpPerspective(original, adjusted, transformMatrix, cv::Size(boxSide, boxSide), cv::INTER_LINEAR, cv::BORDER_CONSTANT);
}

void Recognizer::GetLargestContourFromContours(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largest_contour) {
    std::vector<cv::Point> c_approx;
    int largest_area = 0;

    // go through each point in the contour
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

std::vector<int> Recognizer::GetGrid() {
    if (_grid.empty()) {
        throw std::runtime_error("Grid has not been initialized");
    }
    if (_grid.size() != 81) {
        throw std::runtime_error("Invalid grid size: " + std::to_string(_grid.size()));
    }
    
    // Validate all numbers are in range 0-9
    if (!std::all_of(_grid.begin(), _grid.end(), 
        [](int n) { return n >= 0 && n <= 9; })) {
        throw std::runtime_error("Grid contains invalid numbers");
    }

    // Add validation for initial grid consistency
    for (int i = 0; i < 81; i++) {
        if (_grid[i] != 0) {  // Skip empty cells
            int temp = _grid[i];
            _grid[i] = 0;  // Temporarily remove number to check if it's valid
            if (!IsValidMove(_grid, i, temp)) {
                throw std::runtime_error("Invalid initial grid: conflicting numbers at position " + std::to_string(i));
            }
            _grid[i] = temp;  // Restore the number
        }
    }

    return _grid;
}

} // namespace Sudoku