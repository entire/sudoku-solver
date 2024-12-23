import torch
from torch import nn, optim
from torch.optim.lr_scheduler import ReduceLROnPlateau
from torch.utils.data import DataLoader
from torchvision import transforms
from dataset import get_mnist_dataset
from model import SudokuNet
from tqdm import tqdm
import torch.backends.mps as mps
from utils.logging_config import setup_logger
import os
from visualize import plot_training_history, plot_confusion_matrix

def get_training_transforms():
    return transforms.Compose([
        transforms.RandomAffine(
            degrees=15,
            translate=(0.1, 0.1),
            scale=(0.8, 1.2),
            shear=15
        ),
        transforms.ToTensor(),
        transforms.Normalize((0.5,), (0.5,))
    ])

def train_model():
    logger = setup_logger("train")
    
    # Check if MPS is available
    device = torch.device("mps" if mps.is_available() else "cpu")
    logger.info(f"Using device: {device}")

    # Use separate transforms for training and validation
    train_transform = get_training_transforms()
    val_transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.5,), (0.5,))
    ])

    train_loader = get_mnist_dataset(train=True, transform=train_transform)
    val_loader = get_mnist_dataset(train=False, transform=val_transform)

    # Initialize model, loss, and optimizer
    model = SudokuNet().to(device)  # Move model to MPS device
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=0.001)

    # Initialize tracking lists
    loss_history = []
    val_loss_history = []
    acc_history = []
    val_acc_history = []

    # Increase training epochs
    num_epochs = 20
    
    # Add learning rate scheduling
    scheduler = ReduceLROnPlateau(optimizer, mode='min', factor=0.1, patience=3)
    
    # Add early stopping
    best_val_loss = float('inf')
    patience = 5
    patience_counter = 0
    
    for epoch in range(num_epochs):
        model.train()
        epoch_loss = 0
        correct = 0
        total = 0
        progress_bar = tqdm(train_loader, desc=f"Epoch {epoch+1}")
        
        for images, labels in progress_bar:
            # Move data to MPS device
            images = images.to(device)
            labels = labels.to(device)

            # zero the gradients
            optimizer.zero_grad()
            # predict the outputs
            outputs = model(images)
            # calculate loss function
            loss = criterion(outputs, labels)
            # backpropagate the loss
            loss.backward()
            # update the weights
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

        # Validation phase
        model.eval()
        val_loss = 0
        val_correct = 0
        val_total = 0
        
        with torch.no_grad():
            for images, labels in val_loader:
                images = images.to(device)
                labels = labels.to(device)
                outputs = model(images)
                loss = criterion(outputs, labels)
                
                val_loss += loss.item()
                _, predicted = torch.max(outputs.data, 1)
                val_total += labels.size(0)
                val_correct += (predicted == labels).sum().item()
        
        avg_val_loss = val_loss / len(val_loader)
        val_accuracy = 100 * val_correct / val_total
        
        # Learning rate scheduling
        scheduler.step(avg_val_loss)
        
        # Early stopping
        if avg_val_loss < best_val_loss:
            best_val_loss = avg_val_loss
            patience_counter = 0
            
            # Save state dict version for Python reloading
            torch.save(model.state_dict(), "../models/sudokunet_best_weights.pth")
            
            # Save TorchScript version for C++
            model.eval()
            example_input = torch.randn(1, 1, 28, 28).to(device)
            traced_model = torch.jit.trace(model, example_input)
            torch.jit.save(traced_model, "../models/sudokunet_best.pth")
        else:
            patience_counter += 1
            if patience_counter >= patience:
                print(f"Early stopping triggered after {epoch + 1} epochs")
                break

    # Load best model weights (not the TorchScript version)
    model.load_state_dict(torch.load("../models/sudokunet_best_weights.pth"))

    # Create models directory if it doesn't exist
    os.makedirs("../models", exist_ok=True)
    
    logger.info("Training completed. Saving model...")
    # Save model weights (optional, for Python reloading)
    torch.save(model.state_dict(), "../models/sudokunet_weights.pth")
    
    # Save TorchScript model for C++ inference
    model.eval()  # Switch to evaluation mode
    example_input = torch.randn(1, 1, 28, 28).to(device)  # Adjust size to match your input
    traced_model = torch.jit.trace(model, example_input)
    torch.jit.save(traced_model, "../models/sudokunet.pth")
    logger.info("Model saved in TorchScript format")

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
