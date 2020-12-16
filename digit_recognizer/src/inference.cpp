#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <torch/torch.h>
#include <torch/script.h>
#include "recognizer/inference.h"

namespace Sudoku {

Inference::Inference(/* args */) {}

void Inference::Setup() {
    // load pytorch model
    _module = torch::jit::load("../digit_classifier/models/converted_model.pt");
    _module.eval();
} 

int Inference::GetOutputFromImage(cv::Mat image) {
    PrepImage(image);
    auto tensor_image = torch::from_blob(image.data, {1, 1, image.rows, image.cols}, at::kByte);
    tensor_image = tensor_image.to(at::kFloat);

    // create a vector of inputs
    std::vector<torch::jit::IValue> inputs;
    inputs.emplace_back(tensor_image);
    std::cout << "forward" << std::endl;
    torch::Tensor output = _module.forward(inputs).toTensor();
    std::cout << "output: " << output.slice(/*dim=*/1, /*start=*/0, /*end=*/10) << std::endl;
    std::cout << output.argmax(1) << " " << std::endl;
    return 1;
}

void Inference::PrepImage(cv::Mat &image) {
    cv::Mat cellResizeMat, grayImg, tgtImg;
    cv::resize(image, cellResizeMat, cv::Size(28,28));
    cv::cvtColor(cellResizeMat, grayImg, cv::COLOR_BGR2GRAY);
    grayImg.convertTo(tgtImg, CV_32F, 1.0 / 255.0, 0);
    image = grayImg;
}

Inference::~Inference() {}

} // namespace Sudoku