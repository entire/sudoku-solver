import torch
from torch import nn, optim
from torch.utils.data import DataLoader
from torchvision import transforms
from dataset import get_mnist_dataset
from model import SudokuNet
from tqdm import tqdm
import torch.backends.mps as mps
from utils.logging_config import setup_logger
import os
from visualize import plot_training_history, plot_confusion_matrix

def train_model():
    logger = setup_logger("train")
    
    # Check if MPS is available
    device = torch.device("mps" if mps.is_available() else "cpu")
    logger.info(f"Using device: {device}")

    # Load dataset
    train_loader = get_mnist_dataset(train=True)

    # Initialize model, loss, and optimizer
    model = SudokuNet().to(device)  # Move model to MPS device
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=0.001)

    # Initialize tracking lists
    loss_history = []
    val_loss_history = []
    acc_history = []
    val_acc_history = []

    # Training loop
    for epoch in range(5):
        model.train()
        epoch_loss = 0
        correct = 0
        total = 0
        progress_bar = tqdm(train_loader, desc=f"Epoch {epoch+1}")
        
        for images, labels in progress_bar:
            # Move data to MPS device
            images = images.to(device)
            labels = labels.to(device)
            
            optimizer.zero_grad()
            outputs = model(images)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()
            
            # Track metrics
            epoch_loss += loss.item()
            _, predicted = torch.max(outputs.data, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()
            progress_bar.set_postfix({"loss": f"{loss.item():.4f}"})
            
        # Calculate epoch metrics
        avg_loss = epoch_loss/len(train_loader)
        accuracy = 100 * correct / total
        
        # Store metrics
        loss_history.append(avg_loss)
        acc_history.append(accuracy)
        # For now, use same values for validation metrics since we haven't implemented validation
        val_loss_history.append(avg_loss)
        val_acc_history.append(accuracy)
        
        logger.info(f"Epoch {epoch+1}: Loss = {avg_loss:.4f}, Accuracy = {accuracy:.2f}%")

    # Create models directory if it doesn't exist
    os.makedirs("../models", exist_ok=True)
    
    logger.info("Training completed. Saving model...")
    torch.save(model.state_dict(), "../models/sudokunet.pth")

    history = {
        'loss': loss_history,
        'val_loss': val_loss_history,
        'accuracy': acc_history,
        'val_accuracy': val_acc_history
    }
    
    # After training is complete
    plot_training_history(history)
    
    # After evaluation
    y_true = [...] # true labels
    y_pred = [...] # model predictions
    plot_confusion_matrix(y_true, y_pred)

if __name__ == "__main__":
    train_model()
