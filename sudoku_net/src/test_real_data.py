import torch
from torchvision import transforms
from PIL import Image
import matplotlib.pyplot as plt
from model import SudokuNet
import os

# logging
from utils.logging_config import setup_logger

logger = setup_logger("test_real_data")

def load_and_process_image(image_path):
    # Load and convert to grayscale
    image = Image.open(image_path).convert('L')
    
    # Define transforms similar to training
    transform = transforms.Compose([
        transforms.Resize((28, 28)),
        transforms.ToTensor(),
    ])
    
    return transform(image).unsqueeze(0)  # Add batch dimension

def visualize_predictions():
    # Load the trained model
    model = SudokuNet()
    model.load_state_dict(torch.load('../../models/sudokunet.pth', weights_only=True))
    model.eval()
    
    # Create a figure with subplots
    fig, axes = plt.subplots(3, 4, figsize=(15, 12))
    axes = axes.ravel()
    
    # Process each test cell
    for i in range(12):
        image_path = f'../data/test_data/test_cell{i+1}.jpg'
        
        # Load and process image
        input_tensor = load_and_process_image(image_path)
        
        # Get prediction
        with torch.no_grad():
            output = model(input_tensor)
            predicted = output.argmax(dim=1).item()
        
        # Display image and prediction
        img = Image.open(image_path)
        axes[i].imshow(img, cmap='gray')
        axes[i].set_title(f'Predicted: {predicted}')
        axes[i].axis('off')
    
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    visualize_predictions() 