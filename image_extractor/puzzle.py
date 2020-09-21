from imutils.perspective import four_point_transform
from skimage.segmentation import clear_border
import numpy as np 
import imutils
import cv2

def find_puzzle(image, debug=True):
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

    cv2.imshow("puzzle thresh", not_thresh)
    cv2.waitKey(0)
    
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
            print(puzzle_contour)
            break
    
    if puzzle_contour is None:
        raise Exception("HEY! THIS IS WRONG, could not find puzzle contour")

    # if debug: 
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

def extract_digit(cell, debug=True):
    # apply automatic thresh to cell and then clear any connected borders
    thresh = cv2.threshold(cell, 0, 255, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)[1]
    thresh = clear_border(thresh)

	# find contours in the thresholded cell
    cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    # if no contours were found than this is an empty cell
    if len(cnts) == 0: return None
    # otherwise, find the largest contour in the cell and create a
    # mask for the contour
    c = max(cnts, key=cv2.contourArea)
    mask = np.zeros(thresh.shape, dtype="uint8")
    cv2.drawContours(mask, [c], -1, 255, -1)

    # compute the percentage of masked pixels relative to the total
    # area of the image
    (h, w) = thresh.shape
    percentFilled = cv2.countNonZero(mask) / float(w * h)
    # if less than 3% of the mask is filled then we are looking at
    # noise and can safely ignore the contour
    if percentFilled < 0.03: return None
    # apply the mask to the thresholded cell
    digit = cv2.bitwise_and(thresh, thresh, mask=mask)
    # check to see if we should visualize the masking step
    if debug:
        cv2.imshow("Digit", digit)
        cv2.waitKey(0)
    # return the digit to the calling function
    return digit
