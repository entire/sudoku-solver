//
// Created by kosuke on 9/16/20.
//

#ifndef SUDOKU_RECOGNIZER_H
#define SUDOKU_RECOGNIZER_H

#include <opencv2/core/core.hpp>
#include <vector>
#include "inference.hpp"

namespace Sudoku {
    extern const int maxSize;  // Declaration of the constant

class Recognizer
{
private:
    bool _debug;
    std::vector<int> _grid;
    std::vector<int> _original_grid;
    Inference _inference;
    
    void GetLargestContourFromContours(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largest_contour);
    void FourPointTransform(std::vector<cv::Point>& contour, cv::Mat& original, cv::Mat& adjusted);
    std::vector<int> ProcessImage(cv::Mat& image);

public:
    Recognizer(bool debug = false);
    ~Recognizer();
    
    // Initialize the recognizer and load the ML model
    void Setup();
    
    // Main interface methods
    std::vector<int> RecognizeFromImage(const std::string& imagePath);
    std::vector<int> RecognizeFromFrame(cv::Mat& frame);
    std::vector<int> GetGrid();
    std::vector<int> GetOriginalGrid() const;
    static std::vector<int> SolveSudoku(const std::vector<int>& input_grid);
    
    void SetDebugMode(bool isOn) { 
        _debug = isOn;
        _inference.SetDebugMode(isOn);
    }
    
    void PreprocessCell(const cv::Mat& input, cv::Mat& output);
    double GetCellFillRatio(const cv::Mat& cell);
    bool IsValidMove(const std::vector<int>& grid, int pos, int num);
};

} // namespace Sudoku

#endif //SUDOKU_RECOGNIZER_H
