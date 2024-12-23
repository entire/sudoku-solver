#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <torch/torch.h>
#include <torch/script.h>
#include "recognizer/inference.hpp"

namespace Sudoku {

Inference::Inference(bool debug) : _debug(debug) {}

void Inference::Setup() {
    // load pytorch model
    try {
        // Use absolute path or environment variable for more reliable model loading
        const char* model_path = std::getenv("SUDOKU_MODEL_PATH");
        std::string model_file = model_path ? 
            std::string(model_path) : 
            "../models/sudokunet.pth";
            
        std::filesystem::path abs_path = std::filesystem::absolute(model_file);
        std::cout << "Attempting to load model from: " << abs_path << std::endl;
        std::cout << "File exists: " << std::filesystem::exists(abs_path) << std::endl;
        
        _module = torch::jit::load(model_file);
        std::cout << "Loading model from: " << model_file << std::endl;
        std::cout << "Model loaded successfully" << std::endl;
    } catch (const c10::Error& e) {
        std::cerr << "Error loading the model: " << e.what() << std::endl;
        std::cerr << "My current path is: " << std::filesystem::current_path() << std::endl;
        throw;
    }

    _module.eval();
} 

int Inference::GetOutputFromImage(cv::Mat image) {
    // Increase confidence threshold and add minimum area requirement
    const float CONFIDENCE_THRESHOLD = 0.85;  // Slightly increased
    const float MIN_PIXEL_RATIO = 0.03;  // Minimum ratio of black pixels
    
    // Check if the image has enough content
    double black_pixels = cv::countNonZero(image);
    double total_pixels = image.rows * image.cols;
    double pixel_ratio = black_pixels / total_pixels;
    
    if (pixel_ratio < MIN_PIXEL_RATIO) {
        if (_debug) {
            std::cout << "Cell rejected: insufficient content (ratio: " 
                      << pixel_ratio << ")" << std::endl;
        }
        return 0;
    }
    
    PrepImage(image);
    auto tensor_image = torch::from_blob(image.data, {1, 1, image.rows, image.cols}, at::kByte);
    tensor_image = tensor_image.to(at::kFloat);
    
    auto device = (*_module.parameters().begin()).device();
    tensor_image = tensor_image.to(device);

    std::vector<torch::jit::IValue> inputs;
    inputs.emplace_back(tensor_image);
    torch::Tensor output = _module.forward(inputs).toTensor();
    
    // Get both prediction and confidence
    auto softmax = torch::softmax(output, 1);
    auto confidence = softmax.max().item<float>();
    auto prediction = output.argmax(1).item<int>();
    
    // Get second highest confidence for ambiguity detection
    auto sorted_conf = torch::sort(softmax, 1, true);
    float second_confidence = std::get<0>(sorted_conf)[0][1].item<float>();
    
    // Reject if the difference between top two predictions is too small
    const float MIN_CONFIDENCE_GAP = 0.3;
    if (confidence - second_confidence < MIN_CONFIDENCE_GAP) {
        if (_debug) {
            std::cout << "Cell rejected: ambiguous prediction (confidence gap: " 
                      << confidence - second_confidence << ")" << std::endl;
        }
        return 0;
    }
    
    // Enhanced debug logging
    if (_debug) {
        std::cout << "Digit prediction: " << prediction 
                  << ", Confidence: " << confidence 
                  << ", Second best: " << std::get<1>(sorted_conf)[0][1].item<int>()
                  << " (" << second_confidence << ")" 
                  << ", Pixel ratio: " << pixel_ratio << std::endl;
    }
    
    return confidence > CONFIDENCE_THRESHOLD ? prediction : 0;
}

void Inference::PrepImage(cv::Mat &image) {
    cv::Mat cellResizeMat, grayImg, tgtImg;
    cv::resize(image, cellResizeMat, cv::Size(28,28));
    
    // Check if image is already grayscale
    if (image.channels() == 3) {
        cv::cvtColor(cellResizeMat, grayImg, cv::COLOR_BGR2GRAY);
    } else {
        grayImg = cellResizeMat; // Image is already grayscale
    }
    
    grayImg.convertTo(tgtImg, CV_32F, 1.0 / 255.0, 0);
    image = grayImg;
}

Inference::~Inference() {}

} // namespace Sudoku