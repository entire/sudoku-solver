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

const int maxSize = 450;  // Size for the transformed grid image

Recognizer::Recognizer(bool debug) : _debug(debug), _inference(_debug) {
    _grid.reserve(81);
}

Recognizer::~Recognizer() {}

void Recognizer::Setup() {
    _inference.Setup();
}

std::vector<int> Recognizer::RecognizeFromImage(const std::string& imagePath) {
    cv::Mat image = cv::imread(imagePath);
    if (!image.data) {
        throw std::runtime_error("Could not open image file: " + imagePath);
    }
    return RecognizeFromFrame(image);
}

std::vector<int> Recognizer::RecognizeFromFrame(cv::Mat& frame) {
    if (frame.empty()) {
        throw std::runtime_error("Empty frame provided");
    }
    
    if (frame.rows < 100 || frame.cols < 100) {
        throw std::runtime_error("Frame dimensions too small for reliable recognition");
    }
    
    return ProcessImage(frame);
}

std::vector<int> Recognizer::ProcessImage(cv::Mat& image) {
    cv::Mat resizeImg, gray, blurred, thresh, bitwiseNot, adjusted;
    
    // Improve initial preprocessing
    double scale = 600.0 / std::max(image.rows, image.cols);
    cv::resize(image, resizeImg, cv::Size(), scale, scale);
    
    if (_debug) {
        cv::namedWindow("1. Original", cv::WINDOW_NORMAL);
        cv::imshow("1. Original", resizeImg);
    }
    
    cv::cvtColor(resizeImg, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(7, 7), 3);
    cv::adaptiveThreshold(blurred, thresh, 255, 
                         cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                         cv::THRESH_BINARY, 11, 2.0);
    cv::bitwise_not(thresh, bitwiseNot);

    if (_debug) {
        cv::imshow("2. Preprocessed", bitwiseNot);
        cv::waitKey(100);
    }

    // Find grid contour
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bitwiseNot.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<cv::Point> largest_contour;
    GetLargestContourFromContours(contours, largest_contour);
    
    if (_debug) {
        cv::Mat contour_viz = resizeImg.clone();
        cv::drawContours(contour_viz, std::vector<std::vector<cv::Point>>{largest_contour}, -1, cv::Scalar(0, 255, 0), 2);
        cv::imshow("3. Grid Detection", contour_viz);
    }

    // Transform grid
    FourPointTransform(largest_contour, resizeImg, adjusted);
    
    if (_debug) {
        cv::imshow("4. Transformed Grid", adjusted);
        cv::waitKey(100);
    }

    // After perspective transform, convert to grayscale if not already
    if (adjusted.channels() == 3) {
        cv::cvtColor(adjusted, adjusted, cv::COLOR_BGR2GRAY);
    }
    
    // Create a visualization matrix
    cv::Mat visualization;
    if (_debug) {
        cv::cvtColor(adjusted, visualization, cv::COLOR_GRAY2BGR);
    }

    const int GRID_SIZE = 9;
    double cellWidth = adjusted.cols / static_cast<double>(GRID_SIZE);
    double cellHeight = adjusted.rows / static_cast<double>(GRID_SIZE);

    _grid.clear();
    _grid.reserve(81);
    _original_grid.clear();
    _original_grid.reserve(81);
    
    // Initialize both grids with zeros
    _grid.resize(81, 0);
    _original_grid.resize(81, 0);

    // Create a visualization of all cells
    cv::Mat all_cells = cv::Mat(28 * 9, 28 * 9, CV_8UC1, cv::Scalar(255));

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            int index = row * GRID_SIZE + col;
            
            // Add check for existing number
            if (_grid[index] != 0) {
                if (_debug) {
                    std::cout << "Skipping cell [" << row << "," << col << "] - already contains " 
                             << _grid[index] << std::endl;
                }
                continue;
            }

            if (_debug) {
                std::cout << "\nProcessing cell [" << row << "," << col << "]" << std::endl;
            }

            int x = static_cast<int>(col * cellWidth);
            int y = static_cast<int>(row * cellHeight);
            int w = static_cast<int>(cellWidth);
            int h = static_cast<int>(cellHeight);
            
            // Draw grid lines on visualization
            if (_debug) {
                cv::line(visualization, cv::Point(x, 0), cv::Point(x, adjusted.rows), cv::Scalar(0, 255, 0), 1);
                cv::line(visualization, cv::Point(0, y), cv::Point(adjusted.cols, y), cv::Scalar(0, 255, 0), 1);
            }

            // Extract cell with larger margin to avoid grid lines
            int margin = static_cast<int>(std::min(cellWidth, cellHeight) * 0.12);
            margin = std::min({ margin, w / 6 - 1, h / 6 - 1 });
            margin = std::max(margin, 0);

            cv::Rect cellRect(x + margin, y + margin, w - 2 * margin, h - 2 * margin);
            cellRect &= cv::Rect(0, 0, adjusted.cols, adjusted.rows);

            if (cellRect.width <= 0 || cellRect.height <= 0 || 
                cellRect.width < cellRect.height / 2 || cellRect.height < cellRect.width / 2) {
                _grid[index] = 0;
                continue;
            }
            
            if (_debug) {
                // Draw cell rectangle
                cv::rectangle(visualization, cellRect, cv::Scalar(0, 0, 255), 1);
                cv::imshow("5. Grid Visualization", visualization);
            }

            // Extract and preprocess cell
            cv::Mat cell = adjusted(cellRect);
            cv::Mat cell_processed;
            PreprocessCell(cell, cell_processed);

            // First, check if the cell is empty using computer vision techniques
            double fill_ratio = GetCellFillRatio(cell_processed);
            if (fill_ratio < 0.03) {  // Very low fill ratio indicates empty cell
                if (_debug) {
                    std::cout << "Empty cell detected [" << row << "," << col << "] (fill ratio: " 
                              << fill_ratio << ")" << std::endl;
                }
                _grid[index] = 0;
                continue;
            }

            // Additional check for uniformity/emptiness
            cv::Scalar mean, stddev;
            cv::meanStdDev(cell_processed, mean, stddev);
            if (stddev[0] < 20.0) {  // Low standard deviation indicates uniform (empty) cell
                if (_debug) {
                    std::cout << "Uniform cell detected [" << row << "," << col << "] (stddev: " 
                              << stddev[0] << ")" << std::endl;
                }
                _grid[index] = 0;
                continue;
            }

            // Only proceed with digit recognition if cell appears to have content
            if (fill_ratio > 0.4) {  // Too much content, likely noise or grid lines
                if (_debug) std::cout << "Skipping cell due to excessive fill ratio: " << fill_ratio << std::endl;
                _grid[index] = 0;
                continue;
            }

            // Get prediction with strict confidence threshold
            std::pair<int, float> prediction = _inference.GetOutputWithConfidence(cell_processed);
            int digit = prediction.first;
            float confidence = prediction.second;

            if (confidence < 0.85) {
                if (_debug) std::cout << "Low confidence prediction treated as empty cell: " << confidence << std::endl;
                _grid[index] = 0;
                continue;
            }

            if (_debug) {
                std::cout << "Predicted digit: " << digit << " (confidence: " << confidence << ")" << std::endl;
                // Draw the prediction on visualization
                cv::putText(visualization, 
                          std::to_string(digit),
                          cv::Point(x + w/3, y + 2*h/3),
                          cv::FONT_HERSHEY_SIMPLEX, 
                          0.5,
                          cv::Scalar(0, 255, 0),
                          2);
            }

            _grid[index] = digit;
            _original_grid[index] = digit;

            if (_debug) {
                cv::Mat debug_grid;
                if (adjusted.channels() == 1) {
                    cv::cvtColor(adjusted, debug_grid, cv::COLOR_GRAY2BGR);
                } else {
                    debug_grid = adjusted.clone();
                }
                
                // Draw cell boundaries and predictions
                cv::rectangle(debug_grid, cellRect, cv::Scalar(0, 255, 0), 1);
                cv::putText(debug_grid, std::to_string(digit),
                            cv::Point(x + w/3, y + 2*h/3),
                            cv::FONT_HERSHEY_SIMPLEX, 0.5,
                            cv::Scalar(0, 0, 255), 2);
            }
        }
    }

    if (_debug) {
        cv::imshow("8. Final Grid", visualization);
        cv::waitKey(0);
    }

    return _grid;
}

void Recognizer::FourPointTransform(std::vector<cv::Point>& contour, cv::Mat& original, cv::Mat& adjusted) {
    // Convert to float points
    std::vector<cv::Point2f> corners;
    cv::Mat(contour).convertTo(corners, CV_32F);

    // Find the corners in correct order (top-left, top-right, bottom-right, bottom-left)
    cv::Point2f center = std::accumulate(corners.begin(), corners.end(), cv::Point2f(0.0f, 0.0f)) / 4.0f;
    
    std::vector<cv::Point2f> sorted_corners(4);
    std::vector<cv::Point2f> top, bottom;

    for (const auto& point : corners) {
        if (point.y < center.y)
            top.push_back(point);
        else
            bottom.push_back(point);
    }

    // Sort top points by x coordinate
    if (top[0].x > top[1].x)
        std::swap(top[0], top[1]);
    
    // Sort bottom points by x coordinate
    if (bottom[0].x > bottom[1].x)
        std::swap(bottom[0], bottom[1]);

    sorted_corners[0] = top[0];      // top-left
    sorted_corners[1] = top[1];      // top-right
    sorted_corners[2] = bottom[1];   // bottom-right
    sorted_corners[3] = bottom[0];   // bottom-left

    // Calculate the desired width and height
    float width1 = cv::norm(sorted_corners[1] - sorted_corners[0]);
    float width2 = cv::norm(sorted_corners[2] - sorted_corners[3]);
    float maxWidth = std::max(width1, width2);

    float height1 = cv::norm(sorted_corners[3] - sorted_corners[0]);
    float height2 = cv::norm(sorted_corners[2] - sorted_corners[1]);
    float maxHeight = std::max(height1, height2);

    // Ensure square output
    float size = std::min(maxSize, static_cast<int>(std::max(maxWidth, maxHeight)));

    // Define destination points (square)
    std::vector<cv::Point2f> dst_points = {
        cv::Point2f(0, 0),           // top-left
        cv::Point2f(size - 1, 0),    // top-right
        cv::Point2f(size - 1, size - 1), // bottom-right
        cv::Point2f(0, size - 1)     // bottom-left
    };

    // Get and apply the perspective transform
    cv::Mat transformMatrix = cv::getPerspectiveTransform(sorted_corners, dst_points);
    cv::warpPerspective(original, adjusted, transformMatrix, 
                       cv::Size(size, size),
                       cv::INTER_LINEAR, cv::BORDER_CONSTANT,
                       cv::Scalar(255, 255, 255));

    if (_debug) {
        // Draw the detected corners
        cv::Mat debug = original.clone();
        for (size_t i = 0; i < sorted_corners.size(); i++) {
            cv::circle(debug, sorted_corners[i], 5, cv::Scalar(0, 255, 0), -1);
            cv::putText(debug, std::to_string(i), 
                       cv::Point(sorted_corners[i].x + 5, sorted_corners[i].y + 5),
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
        }
        cv::imshow("Corner Detection", debug);
    }
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

    Sudoku::Solver solver;
    if (!solver.ValidateGrid(_grid, _debug)) {
        throw std::runtime_error("Invalid grid detected");
    }

    return _grid;
}

void Recognizer::PreprocessCell(const cv::Mat& input, cv::Mat& output) {
    cv::Mat gray;
    if (input.channels() == 3) {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    } else {
        input.copyTo(gray);
    }

    // Enhance contrast
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0, cv::Size(8,8));
    clahe->apply(gray, gray);

    // Denoise
    cv::medianBlur(gray, gray, 3);

    // Adaptive thresholding with more aggressive parameters
    cv::adaptiveThreshold(gray, output, 255, 
                         cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                         cv::THRESH_BINARY_INV, 11, 4);

    // Clean up noise
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2,2));
    cv::morphologyEx(output, output, cv::MORPH_OPEN, kernel);

    // Add padding and resize
    int padding = 4;
    cv::copyMakeBorder(output, output, padding, padding, padding, padding, 
                      cv::BORDER_CONSTANT, cv::Scalar(0));
    cv::resize(output, output, cv::Size(28, 28), 0, 0, cv::INTER_AREA);
}

double Recognizer::GetCellFillRatio(const cv::Mat& cell) {
    return static_cast<double>(cv::countNonZero(cell)) / (cell.rows * cell.cols);
}

std::vector<int> Recognizer::GetOriginalGrid() const {
    return _original_grid;
}

} // namespace Sudoku