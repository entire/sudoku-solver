import torch
import cv2
import os
import numpy as np
from sudoku_net.src.model import SudokuNet
from sudoku_net.src.utils.logging_config import setup_logger

# Setup logging configuration
logger = setup_logger(__name__)

class Inference:
    def __init__(self):
        self._model = None

    def setup(self):
        """Load the PyTorch model."""
        logger.info("Setting up inference model")
        model_path = os.path.join(os.path.dirname(__file__), "..", "sudoku_net", "models", "sudokunet.pth")
        self._model = SudokuNet()  # Initialize the same model architecture used in training
        self.load_model(model_path)
        self._model.eval()
        logger.info("Model setup complete")

    def load_model(self, model_path):
        if not os.path.exists(model_path):
            logger.error(f"Model file not found: {model_path}")
            raise ValueError(f"Model file not found: {model_path}")
        
        logger.debug(f"Loading model from {model_path}")
        self._model.load_state_dict(
            torch.load(
                model_path,
                map_location=torch.device('cpu'),
                weights_only=True  # Address the PyTorch warning
            )
        )
        logger.info("Model loaded successfully")

    def get_output_from_image(self, image):
        """Process image and get model prediction."""
        logger.debug("Processing image for prediction")
        # Preprocess the image
        processed_image = self.prep_image(image)
        
        # Convert to tensor
        tensor_image = torch.from_numpy(processed_image).float()
        tensor_image = tensor_image.unsqueeze(0).unsqueeze(0)  # Add batch and channel dimensions

        # Get prediction
        with torch.no_grad():
            output = self._model(tensor_image)
            prediction = output.argmax(1).item()
            
        return prediction

    @staticmethod
    def prep_image(image):
        """Prepare image for model input."""
        # Ensure image is grayscale
        if len(image.shape) > 2:
            image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            
        # Resize to 28x28
        image = cv2.resize(image, (28, 28))
        
        # Normalize pixel values
        image = image.astype(np.float32) / 255.0
        
        return image
