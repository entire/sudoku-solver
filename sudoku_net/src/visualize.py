import seaborn as sns
import matplotlib.pyplot as plt
from dataset import get_mnist_dataset
from utils.logging_config import setup_logger
from sklearn.metrics import confusion_matrix

def visualize_samples(num_samples=16):
    logger = setup_logger("visualize")
    
    logger.info("Loading samples for visualization...")
    data_loader = get_mnist_dataset(train=True, batch_size=16)
    data_iter = iter(data_loader)
    images, labels = next(data_iter)

    # Plot images
    fig, axes = plt.subplots(2, 8, figsize=(10, 5))
    for i, ax in enumerate(axes.flat):
        ax.imshow(images[i][0], cmap="gray")
        ax.set_title(f"Label: {labels[i]}")
        ax.axis("off")
    plt.show()
    
    logger.info("Visualization complete")

def plot_training_history(history):
    """
    Plot training and validation metrics over epochs.
    Args:
        history: Dictionary containing lists of metrics (loss, accuracy, etc.)
    """
    logger = setup_logger("visualize")
    logger.info("Plotting training history...")
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))
    
    # Plot loss
    ax1.plot(history['loss'], label='Training Loss')
    ax1.plot(history['val_loss'], label='Validation Loss')
    ax1.set_title('Model Loss')
    ax1.set_xlabel('Epoch')
    ax1.set_ylabel('Loss')
    ax1.legend()
    
    # Plot accuracy
    ax2.plot(history['accuracy'], label='Training Accuracy')
    ax2.plot(history['val_accuracy'], label='Validation Accuracy')
    ax2.set_title('Model Accuracy')
    ax2.set_xlabel('Epoch')
    ax2.set_ylabel('Accuracy')
    ax2.legend()
    
    plt.tight_layout()
    plt.show()

def plot_confusion_matrix(y_true, y_pred, class_names=None):
    """
    Plot confusion matrix for model evaluation.
    Args:
        y_true: True labels
        y_pred: Predicted labels
        class_names: List of class names (optional)
    """
    logger = setup_logger("visualize")
    logger.info("Plotting confusion matrix...")
    
    cm = confusion_matrix(y_true, y_pred)
    plt.figure(figsize=(10, 8))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                xticklabels=class_names,
                yticklabels=class_names)
    plt.title('Confusion Matrix')
    plt.xlabel('Predicted')
    plt.ylabel('True')
    plt.show()

if __name__ == "__main__":
    visualize_samples()
