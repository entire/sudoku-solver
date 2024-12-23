# import the necessary packages
from recognizer.recognizer import Recognizer
from sudoku import Solver, GameState
from overlayer.overlayer import Overlayer
import argparse
import cv2
import os
import logging

# Initialize logger
logger = logging.getLogger(__name__)

# Setup logging configuration
logging.basicConfig(
	level=logging.INFO,
	format='[%(asctime)s] - %(levelname)s - %(message)s',
	datefmt='%m%d%H:%M:%S:%s'
)

def start_video(recognizer):
	# Initialize video capture
	camera = cv2.VideoCapture(0)
	if not camera.isOpened():
		logging.error("Couldn't open camera")
		return

	solver = Solver()  # Initialize the Sudoku solver

	while True:
		# Read frame
		success, frame = camera.read()
		if not success:
			logging.info("This is the end of the video")
			break

		# Process frame with recognizer
		recognizer.process_frame(frame)
		puzzle_grid = recognizer.get_grid()
		
		if puzzle_grid:
			# Flatten the 2D grid into 1D list for solver
			flat_grid = [num for row in puzzle_grid for num in row]
			
			# Solve the puzzle
			solved_game = solver.solve(flat_grid)
			if solved_game and solved_game.state == GameState.complete:
				# Draw the solution on the frame
				puzzle_image = recognizer.get_puzzle_image()
				cell_locations = recognizer.get_cell_locations()
				if puzzle_image is not None:
					Overlayer.draw_solution(puzzle_image, solved_game, cell_locations)
					cv2.imshow("Solved Puzzle", puzzle_image)

		# Show the original frame
		cv2.imshow("Camera Feed", frame)

		# Press 'q' to quit
		if cv2.waitKey(1) & 0xFF == ord('q'):
			break

	camera.release()
	cv2.destroyAllWindows()

def process_image(recognizer, image_path):
	# Verify image exists
	if not os.path.exists(image_path):
		logging.error(f"Image file not found: {image_path}")
		return

	# Read the image
	logging.info(f"Processing image: {image_path}")
	image = cv2.imread(image_path)
	if image is None:
		logging.error(f"Couldn't read image: {image_path}")
		return

	# Process image with recognizer
	recognizer.process_frame(image)
	puzzle_grid = recognizer.get_grid()
	
	if puzzle_grid:
		# Initialize solver and solve the puzzle
		solver = Solver()
		flat_grid = [num for row in puzzle_grid for num in row]
		solved_game = solver.solve(flat_grid)
		
		if solved_game and solved_game.state == GameState.complete:
			# Draw the solution
			puzzle_image = recognizer.get_puzzle_image()
			cell_locations = recognizer.get_cell_locations()
			if puzzle_image is not None:
				Overlayer.draw_solution(puzzle_image, solved_game, cell_locations)
				cv2.imshow("Solved Puzzle", puzzle_image)
				cv2.waitKey(0)

	cv2.destroyAllWindows()

def main():
	# construct the argument parser and parse the arguments
	ap = argparse.ArgumentParser()
	ap.add_argument("-d", "--debug", type=int, default=0,
		help="whether or not we are visualizing each step of the pipeline")
	ap.add_argument("-i", "--image", type=str,
		help="path to input image file")
	args = vars(ap.parse_args())

	# Initialize and setup the recognizer
	logging.info("Loading digit classifier...")
	try:
		recognizer = Recognizer()
		recognizer.set_debug_mode(args["debug"] > 0)
		recognizer.setup()
	except ValueError as e:
		logging.error(f"Error during setup: {e}")
		return
	except Exception as e:
		logging.error(f"Unexpected error during setup: {e}")
		return

	# Process image if provided, otherwise start video capture
	if args["image"]:
		process_image(recognizer, args["image"])
	else:
		start_video(recognizer)

if __name__ == "__main__":
	main()