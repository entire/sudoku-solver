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
    torch::jit::script::Module _module;
public:
    Inference(/* args */);
    ~Inference();

    void Setup();
    int GetOutputFromImage(cv::Mat image);
    void PrepImage(cv::Mat &image);
};


} // namespace Sudoku

#endif // SUDOKU_INFERENCE_H
