#include "recognizer/overlayer.hpp"
#include <opencv2/imgproc/imgproc.hpp>

namespace Sudoku {

Overlayer::Overlayer() {}

Overlayer::~Overlayer() {}

void Overlayer::OverlayAnswers(cv::Mat& image, const std::vector<int>& grid, const std::vector<int>& original_grid, cv::Scalar color) {
    if (grid.size() != 81 || original_grid.size() != 81) {
        throw std::runtime_error("Grid and original grid must contain exactly 81 cells");
    }

    // Calculate cell dimensions based on image size
    int cellWidth = image.cols / 9;
    int cellHeight = image.rows / 9;

    // Overlay each digit
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int index = i * 9 + j;
            int digit = grid[index];
            int original = original_grid[index];
            
            // Only draw if it's a new number (wasn't in original grid)
            if (digit != 0 && original == 0) {
                cv::Point position(j * cellWidth + cellWidth/2, 
                                 (i + 1) * cellHeight - cellHeight/3);
                DrawDigit(image, digit, position, color);
            }
        }
    }
}

void Overlayer::DrawDigit(cv::Mat& image, int digit, const cv::Point& position, cv::Scalar color) {
    // Calculate appropriate font scale based on cell size
    double fontScale = (double)image.rows / (25.0 * 9);  // Slightly smaller text
    
    // Draw text with outline for better visibility
    int thickness = std::max(1, (int)(fontScale * 2));
    
    // Draw outline
    cv::putText(image, 
                std::to_string(digit), 
                position,
                cv::FONT_HERSHEY_DUPLEX,  // Changed font for better readability
                fontScale,
                cv::Scalar(255, 255, 255),  // White outline
                thickness + 2,
                cv::LINE_AA,
                false);  // Center-aligned text
    
    // Draw main text
    cv::putText(image, 
                std::to_string(digit), 
                position,
                cv::FONT_HERSHEY_DUPLEX,
                fontScale,
                color,
                thickness,
                cv::LINE_AA,
                false);
}

} // namespace Sudoku
