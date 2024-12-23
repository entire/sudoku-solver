#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <torch/torch.h>
#include <torch/script.h>
#include "recognizer/inference.hpp"

namespace Sudoku {

Inference::Inference(bool debug) : _debug(debug) {}

void Inference::Setup() {
    std::string model_file;
    try {
        const char* model_path = std::getenv("SUDOKU_MODEL_PATH");
        model_file = model_path ? 
            std::string(model_path) : 
            "../models/sudokunet_best.pth";  // Use the best model from training
            
        // Print current working directory
        std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
        
        // Get and print absolute path of the model file
        std::filesystem::path abs_path = std::filesystem::absolute(model_file);
        std::cout << "Attempting to load model from: " << abs_path << std::endl;
        std::cout << "Model file exists: " << std::filesystem::exists(abs_path) << std::endl;
        
        // Print parent directory contents to help debug
        std::cout << "Contents of parent directory:" << std::endl;
        for (const auto& entry : std::filesystem::directory_iterator(abs_path.parent_path())) {
            std::cout << "  " << entry.path().filename() << std::endl;
        }
        
        _module = torch::jit::load(model_file);
        _module.eval();  // Ensure model is in evaluation mode
        
        if (_debug) {
            std::cout << "Model loaded successfully" << std::endl;
        }
    } catch (const c10::Error& e) {
        std::cerr << "Looking for model at: " << model_file << std::endl;
        std::cerr << "Error loading the model: " << e.what() << std::endl;
        std::cerr << "Current path: " << std::filesystem::current_path() << std::endl;
        throw;
    }
}

std::pair<int, float> Inference::GetOutputWithConfidence(cv::Mat image) {
    PrepImage(image);  // Prepare image for inference
    
    auto tensor_image = torch::from_blob(image.data, {1, 1, image.rows, image.cols}, at::kByte);
    tensor_image = tensor_image.to(at::kFloat).div(255.0);  // Normalize to [0,1]
    tensor_image = tensor_image.sub(0.5).div(0.5);  // Normalize to [-1,1] as in training
    
    auto device = (*_module.parameters().begin()).device();
    tensor_image = tensor_image.to(device);

    std::vector<torch::jit::IValue> inputs;
    inputs.emplace_back(tensor_image);
    
    torch::Tensor output;
    {
        torch::NoGradGuard no_grad;  // Disable gradient computation for inference
        output = _module.forward(inputs).toTensor();
    }
    
    auto softmax = torch::softmax(output, 1);
    auto confidence = softmax.max().item<float>();
    auto prediction = output.argmax(1).item<int>();
    
    // Get top-k predictions for debugging
    if (_debug) {
        auto topk = torch::topk(softmax, 3, 1);
        auto values = std::get<0>(topk)[0];
        auto indices = std::get<1>(topk)[0];
        
        std::cout << "Top 3 predictions:" << std::endl;
        for (int i = 0; i < 3; i++) {
            std::cout << "  " << indices[i].item<int>() 
                     << ": " << values[i].item<float>() << std::endl;
        }
    }
    
    return std::make_pair(prediction, confidence);
}

void Inference::PrepImage(cv::Mat &image) {
    if (image.empty()) {
        throw std::runtime_error("Empty image provided to PrepImage");
    }

    // Ensure consistent image size
    cv::resize(image, image, cv::Size(28, 28), 0, 0, cv::INTER_AREA);
    
    // Convert to grayscale if needed
    if (image.channels() == 3) {
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    }
    
    // Ensure binary image
    if (image.type() != CV_8UC1) {
        image.convertTo(image, CV_8UC1);
    }
    
    if (_debug) {
        cv::imshow("Preprocessed Cell", image);
        cv::waitKey(1);
    }
}

Inference::~Inference() {}

} // namespace Sudoku