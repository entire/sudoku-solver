from torch.utils.data import DataLoader
from torchvision import datasets, transforms

def get_mnist_dataset(train=True, transform=None, batch_size=32):
    """
    Get MNIST dataset loader
    Args:
        train (bool): If True, returns training dataset, else test dataset
        transform: Optional transform to be applied to the data
        batch_size (int): Batch size for the data loader
    """
    dataset = datasets.MNIST(
        root='../data',
        train=train,
        download=True,
        transform=transform
    )
    
    return DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=train,  # Shuffle only training data
        num_workers=2
    )
