//
// Created by kosuke on 9/27/20.
//

#ifndef SUDOKU_INFERENCE_H
#define SUDOKU_INFERENCE_H

#include <opencv2/core/core.hpp>
#include <torch/torch.h>
#include <torch/script.h>

namespace Sudoku {
class Inference
{
private:
    /* data */
    torch::jit::Module _module;
    bool _debug = false;
public:
    Inference(bool debug = false);
    ~Inference();

    void Setup();
    std::pair<int, float> GetOutputWithConfidence(cv::Mat image);
    void PrepImage(cv::Mat &image);
    void SetDebugMode(bool isOn) {
        _debug = isOn;
    }
};


} // namespace Sudoku

#endif // SUDOKU_INFERENCE_H
