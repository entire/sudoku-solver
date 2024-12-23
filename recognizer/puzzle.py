from typing import Tuple, Optional
from imutils.perspective import four_point_transform
from skimage.segmentation import clear_border
import numpy as np 
import imutils
import cv2
from sudoku_net.src.utils.logging_config import setup_logger

# Setup logging for this module
logger = setup_logger(__name__)

# Constants
GAUSSIAN_KERNEL_SIZE = (7, 7)
GAUSSIAN_SIGMA = 3
ADAPTIVE_BLOCK_SIZE = 11
ADAPTIVE_CONSTANT = 2
CONTOUR_APPROX_FACTOR = 0.02
MIN_DIGIT_AREA_PERCENT = 0.03

def preprocess_image(image: np.ndarray) -> np.ndarray:
    """Convert image to grayscale and apply blur and thresholding."""
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, GAUSSIAN_KERNEL_SIZE, GAUSSIAN_SIGMA)
    thresh = cv2.adaptiveThreshold(
        blurred, 
        255, 
        cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
        cv2.THRESH_BINARY, 
        ADAPTIVE_BLOCK_SIZE, 
        ADAPTIVE_CONSTANT
    )
    return cv2.bitwise_not(thresh, thresh)

def find_puzzle_contour(thresh_image: np.ndarray) -> np.ndarray:
    """Find and return the puzzle contour from the thresholded image."""
    contours = cv2.findContours(
        thresh_image.copy(),
        cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE)
    contours = imutils.grab_contours(contours)
    contours = sorted(contours, key=cv2.contourArea, reverse=True)

    for contour in contours:
        perimeter = cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, CONTOUR_APPROX_FACTOR * perimeter, True)

        if len(approx) == 4:
            return approx
    
    raise ValueError("Could not find puzzle contour - ensure image contains a valid puzzle")

def find_puzzle(image: np.ndarray, debug: bool = True) -> Tuple[np.ndarray, np.ndarray]:
    """Find and return the puzzle in both RGB and grayscale formats."""
    logger.info("Starting puzzle detection")
    
    # Preprocess the image
    thresh = preprocess_image(image)
    logger.debug("Image preprocessing complete")

    try:
        # Find the puzzle contour
        puzzle_contour = find_puzzle_contour(thresh)
        logger.debug("Puzzle contour found")
    except ValueError as e:
        logger.error("Failed to find puzzle contour: %s", str(e))
        raise

    if debug:
        cv2.imshow("puzzle thresh", thresh)
        cv2.waitKey(0)
    
    # Debug visualization
    if debug:
        output = image.copy()
        cv2.drawContours(output, [puzzle_contour], -1, (0, 255, 0), 2)
        cv2.imshow("puzzle outline", output)
        cv2.waitKey(0)

    # Transform the puzzle
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    puzzle = four_point_transform(image, puzzle_contour.reshape(4, 2))
    warped = four_point_transform(gray, puzzle_contour.reshape(4, 2))

    if debug:
        cv2.imshow("Puzzle Transform", warped)
        cv2.waitKey(0)

    return (puzzle, warped)

def extract_digit(cell: np.ndarray, debug: bool = True) -> Optional[np.ndarray]:
    """Extract and return the digit from a cell, or None if no digit is found."""
    # apply automatic thresh to cell and then clear any connected borders
    thresh = cv2.threshold(cell, 0, 255, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)[1]
    thresh = clear_border(thresh)

    # find contours in the thresholded cell
    cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    
    if len(cnts) == 0:
        return None

    # Find the largest contour and create its mask
    largest_contour = max(cnts, key=cv2.contourArea)
    mask = np.zeros(thresh.shape, dtype="uint8")
    cv2.drawContours(mask, [largest_contour], -1, 255, -1)

    # Check if the contour is large enough to be a digit
    height, width = thresh.shape
    percent_filled = cv2.countNonZero(mask) / float(width * height)
    
    if percent_filled < MIN_DIGIT_AREA_PERCENT:
        return None

    # Apply the mask to get the final digit
    digit = cv2.bitwise_and(thresh, thresh, mask=mask)

    if debug:
        cv2.imshow("Digit", digit)
        cv2.waitKey(0)

    return digit
