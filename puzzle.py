from imutils.perspective import four_point_transform
from skimage.segmentation import clear_border
import numpy as np 
import imutils
import cv2

def find_puzzle(image, debug=False):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (7,7), 3)
    thresh = cv2.adaptiveThreshold(
        blurred, 
        255, 
        cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
        cv2.THRESH_BINARY, 
        11, 
        2
    )
    not_thresh = cv2.bitwise_not(thresh, thresh)

    # if debug:
        # cv2.imshow("puzzle thresh", not_thresh)
        # cv2.waitKey(0)
    
    # ok cool, now we find some contours
    contours = cv2.findContours(
        not_thresh.copy(),
        cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE)
    contours = imutils.grab_contours(contours)
    contours = sorted(contours, key=cv2.contourArea, reverse=True)

    # init a contour that corresponds to puzzle outline
    puzzle_contour = None

    for c in contours:
        perimeter = cv2.arcLength(c, True)
        approx = cv2.approxPolyDP(c, 0.02 * perimeter, True)

        if len (approx) == 4:
            print("YUP LOOKS LIKE IT")
            puzzle_contour = approx
            break
    
    if puzzle_contour is None:
        raise Exception("HEY! THIS IS WRONG, could not find puzzle contour")

    if debug: 
        output = image.copy()
        cv2.drawContours(output, [puzzle_contour], -1, (0, 255, 0), 2)
        cv2.imshow("puzzle outline", output)
        cv2.waitKey(0)

    # get the 4pt transform of outline contour
    puzzle = four_point_transform(image, puzzle_contour.reshape(4, 2))
    warped = four_point_transform(gray, puzzle_contour.reshape(4, 2))
    # check to see if we are visualizing the perspective transform
    if debug:
        # show the output warped image (again, for debugging purposes)
        cv2.imshow("Puzzle Transform", warped)
        cv2.waitKey(0)
    # return a 2-tuple of puzzle in both RGB and grayscale
    return (puzzle, warped)

if __name__ == "__main__":
    image = cv2.imread('digit_classifier/assets/puzzle.jpg')
    find_puzzle(image, True)