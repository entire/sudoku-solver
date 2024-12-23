import cv2
import numpy as np
from .puzzle import find_puzzle, extract_digit
from .inference import Inference
import os
from sudoku_net.src.utils.logging_config import setup_logger

# setup logging
logger = setup_logger(__name__)

class Recognizer:
    def __init__(self):
        self._debug = False
        self._grid = []
        self._inference = Inference()
        self._puzzle_image = None
        self._cell_locations = []

    def setup(self):
        """Initialize the recognizer and load the model."""
        logger.info("Initializing recognizer !")
        try:
            test_image_path = os.path.join(os.path.dirname(__file__), "assets", "sudoku_1.jpg")
            logger.debug(f"Test image path: {test_image_path}")
            
            if not os.path.exists(test_image_path):
                logger.error(f"Test image not found at: {test_image_path}")
                raise ValueError(f"Test image not found at: {test_image_path}")
            
            test_image = cv2.imread(test_image_path)
            if test_image is None:
                logger.error("Failed to load test image")
                raise ValueError("Failed to load test image")
            
            logger.info("Test image loaded successfully")
            self._inference.setup()
            
            # Use sudoku_1.jpg instead of sudoku_12.jpg
            image = test_image  # Reuse the test image we already loaded
            
            # Resize image
            image = cv2.resize(image, None, fx=0.75, fy=0.75)

            # Find the puzzle in the image
            puzzle, warped = find_puzzle(image, debug=self._debug)
            self._puzzle_image = puzzle

            # Initialize 9x9 grid
            grid = []
            cell_locs = []
            step_y = warped.shape[0] // 9
            step_x = warped.shape[1] // 9

            # Loop over grid locations
            for y in range(9):
                row = []
                cell_row = []
                for x in range(9):
                    # Calculate cell coordinates
                    start_x = x * step_x
                    start_y = y * step_y
                    end_x = (x + 1) * step_x
                    end_y = (y + 1) * step_y
                    cell_row.append((start_x, start_y, end_x, end_y))

                    # Extract the cell
                    cell = warped[start_y:end_y, start_x:end_x]

                    # Extract the digit from the cell
                    digit = extract_digit(cell, debug=self._debug)
                    
                    if digit is not None:
                        # Get prediction from the model
                        prediction = self._inference.get_output_from_image(digit)
                        row.append(prediction)
                    else:
                        row.append(0)  # Empty cell
                        
                grid.append(row)
                cell_locs.append(cell_row)

            self._grid = grid
            self._cell_locations = cell_locs

            logger.info("Recognizer initialization complete")
            
        except Exception as e:
            logger.error(f"Setup failed: {str(e)}")
            raise ValueError(f"Setup failed: {str(e)}")

    def set_debug_mode(self, is_on):
        """Set debug mode."""
        self._debug = is_on

    def get_grid(self):
        """Return the recognized grid."""
        return self._grid

    def get_puzzle_image(self):
        """Return the processed puzzle image."""
        return self._puzzle_image

    def get_cell_locations(self):
        """Return the cell locations."""
        return self._cell_locations

    def process_frame(self, frame):
        """Process a new frame or image."""
        # Resize frame
        frame = cv2.resize(frame, None, fx=0.75, fy=0.75)

        # Find the puzzle in the image
        puzzle, warped = find_puzzle(frame, debug=self._debug)
        if puzzle is None or warped is None:
            return False
        
        self._puzzle_image = puzzle

        # Initialize 9x9 grid
        grid = []
        cell_locs = []
        step_y = warped.shape[0] // 9
        step_x = warped.shape[1] // 9

        # Loop over grid locations
        for y in range(9):
            row = []
            cell_row = []
            for x in range(9):
                # Calculate cell coordinates
                start_x = x * step_x
                start_y = y * step_y
                end_x = (x + 1) * step_x
                end_y = (y + 1) * step_y
                cell_row.append((start_x, start_y, end_x, end_y))

                # Extract the cell
                cell = warped[start_y:end_y, start_x:end_x]

                # Extract the digit from the cell
                digit = extract_digit(cell, debug=self._debug)
                
                if digit is not None:
                    # Get prediction from the model
                    prediction = self._inference.get_output_from_image(digit)
                    row.append(prediction)
                else:
                    row.append(0)  # Empty cell
                    
            grid.append(row)
            cell_locs.append(cell_row)

        self._grid = grid
        self._cell_locations = cell_locs
        return True
