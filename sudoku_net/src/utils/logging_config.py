import logging
import sys
from pathlib import Path

def setup_logger(name: str) -> logging.Logger:
    # Create logs directory if it doesn't exist
    log_dir = Path("../logs")
    log_dir.mkdir(exist_ok=True)
    
    # Create logger
    logger = logging.getLogger(name)
    logger.setLevel(logging.INFO)
    
    # Create formatters
    file_formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    console_formatter = logging.Formatter('%(message)s')
    
    # File handler
    file_handler = logging.FileHandler(log_dir / f"{name}.log")
    file_handler.setFormatter(file_formatter)
    
    # Add handlers
    logger.addHandler(file_handler)
    
    return logger 