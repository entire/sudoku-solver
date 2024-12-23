#include "recognizer/overlayer.hpp"
#include <opencv2/imgproc/imgproc.hpp>

namespace Sudoku {

Overlayer::Overlayer() {}

Overlayer::~Overlayer() {}

void Overlayer::OverlayAnswers(cv::Mat& image, const std::vector<int>& grid, cv::Scalar color) {
    if (grid.size() != 81) {
        throw std::runtime_error("Grid must contain exactly 81 cells");
    }

    // Calculate cell dimensions based on image size
    int cellWidth = image.cols / 9;
    int cellHeight = image.rows / 9;

    // Overlay each digit
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int digit = grid[i * 9 + j];
            if (digit != 0) {  // Only draw non-empty cells
                cv::Point position(j * cellWidth + cellWidth/3, 
                                 (i + 1) * cellHeight - cellHeight/3);
                DrawDigit(image, digit, position, color);
            }
        }
    }
}

void Overlayer::DrawDigit(cv::Mat& image, int digit, const cv::Point& position, cv::Scalar color) {
    // Calculate appropriate font scale based on cell size
    double fontScale = (double)image.rows / (20.0 * 9);
    
    cv::putText(image, 
                std::to_string(digit), 
                position,
                cv::FONT_HERSHEY_SIMPLEX, 
                fontScale,
                color,
                FONT_THICKNESS,
                cv::LINE_AA);
}

} // namespace Sudoku
