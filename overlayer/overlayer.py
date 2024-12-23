from sudoku_net.src.utils.logging_config import setup_logger

# Setup logging for this module
logger = setup_logger(__name__)

import cv2
import numpy as np

class Overlayer:
    @staticmethod
    def draw_solution(image, solved_game, cell_locations):
        """Draw the solved numbers on the puzzle image."""
        if image is None or solved_game is None or cell_locations is None:
            logger.error("Invalid inputs for draw_solution")
            return

        logger.info("Drawing solution overlay")
        height, width = image.shape[:2]
        font = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = min(width, height) / 400.0
        thickness = max(1, int(min(width, height) / 200.0))

        for i, row in enumerate(cell_locations):
            for j, (x1, y1, x2, y2) in enumerate(row):
                cell_key = chr(ord('A') + i) + str(j + 1)
                if cell_key in solved_game.cells:
                    cell = solved_game.cells[cell_key]
                    if len(cell.candidates) == 1:
                        number = cell.candidates[0]
                        # Calculate text size and position
                        text = str(number)
                        (text_width, text_height), _ = cv2.getTextSize(text, font, font_scale, thickness)
                        text_x = x1 + (x2 - x1 - text_width) // 2
                        text_y = y1 + (y2 - y1 + text_height) // 2
                        cv2.putText(image, text, (text_x, text_y), font, font_scale, (0, 255, 0), thickness)
