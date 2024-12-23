# SudokuNet

A digit classifier using deep learning and the MNIST dataset

## Features
- Uses PyTorch for training a CNN model (SudokuNet)
- Visualizes performance using Seaborn
- Includes scripts for training, evaluation, and visualization
- Comprehensive logging system with both console and file outputs

## Setup
1. Install `uv`:
   ```bash
   pip install uv
   uv init
   ```

2. Install dependencies:
   ```bash
   uv pip install -r requirements.txt
   ```

## Project Structure
```
sudoku_net/
├── logs/           # Generated log files
├── models/         # Saved model weights
└── src/
    ├── utils/
    │   ├── __init__.py
    │   └── logging_config.py
    ├── dataset.py
    ├── model.py
    ├── train.py
    ├── evaluate.py
    └── visualize.py
```

## Usage

Run the scripts in the following order:

1. Train the model:
   ```bash
   uv run src/train.py
   ```
   This will train the model and save it to `models/sudokunet.pth`

2. Evaluate the model:
   ```bash
   uv run src/evaluate.py
   ```
   This will load the trained model and show its accuracy on the test set

3. Visualize samples:
   ```bash
   uv run src/visualize.py
   ```
   This will display a grid of MNIST digits with their labels

4. Test the model on real data:
   ```bash
   uv run src/test_real_data.py
   ```
   This will load the trained model and show its accuracy on the test set

## Logs
All scripts generate logs in the `logs/` directory with the following files:
- `train.log`: Training progress and loss values
- `evaluate.log`: Model evaluation results
- `visualize.log`: Visualization process logs
