#ifndef SUDOKU_OVERLAYER_HPP
#define SUDOKU_OVERLAYER_HPP

#include <opencv2/core/core.hpp>
#include <vector>

namespace Sudoku {

class Overlayer {
public:
    Overlayer();
    ~Overlayer();

    // Overlay the recognized digits onto the original image
    void OverlayAnswers(cv::Mat& image, const std::vector<int>& grid, cv::Scalar color = cv::Scalar(0, 255, 0));

private:
    // Helper method to draw a single digit
    void DrawDigit(cv::Mat& image, int digit, const cv::Point& position, cv::Scalar color = cv::Scalar(0, 255, 0));
    
    // Constants for drawing
    const int CELL_SIZE = 50;  // Will be adjusted based on image size
    const int FONT_SCALE = 1;
    const int FONT_THICKNESS = 2;
    const cv::Scalar DIGIT_COLOR = cv::Scalar(0, 255, 0);  // Green color
};

} // namespace Sudoku

#endif // SUDOKU_OVERLAYER_HPP
