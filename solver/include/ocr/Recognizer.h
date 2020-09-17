//
// Created by kosuke on 9/16/20.
//

#ifndef SUDOKU_RECOGNIZER_H
#define SUDOKU_RECOGNIZER_H

namespace Sudoku {

class Recognizer
{
private:
    // debug mode shows lots of intermediary steps,
    bool _debug = false;
public:
    Recognizer();
    Recognizer(const Recognizer&) = delete;
    ~Recognizer();
    void Setup();
    void GetLargestContourFromContours(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largest_contour);
    void FourPointTransform(std::vector<cv::Point>& contour, cv::Mat& original, cv::Mat& adjusted);
    void SetDebugMode(bool isOn) { _debug = isOn; };
};

} // namespace Sudoku

#endif //SUDOKU_RECOGNIZER_H
