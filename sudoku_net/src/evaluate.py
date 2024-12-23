import torch
from dataset import get_mnist_dataset
from model import SudokuNet
from utils.logging_config import setup_logger
from tqdm import tqdm

def evaluate_model():
    logger = setup_logger("evaluate")
    
    # Load dataset
    test_loader = get_mnist_dataset(train=False)

    # Load model
    model = SudokuNet()
    model.load_state_dict(torch.load("../../models/sudokunet.pth", weights_only=True))
    model.eval()

    logger.info("Starting model evaluation...")
    correct = 0
    total = 0
    with torch.no_grad():
        for images, labels in tqdm(test_loader, desc="Evaluating"):
            outputs = model(images)
            _, predicted = torch.max(outputs, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()
    
    accuracy = 100 * correct / total
    logger.info(f"Evaluation completed. Accuracy: {accuracy:.2f}%")

if __name__ == "__main__":
    evaluate_model()
