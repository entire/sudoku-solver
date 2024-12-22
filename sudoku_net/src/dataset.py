from torch.utils.data import DataLoader
from torchvision import datasets, transforms

def get_mnist_dataset(train=True, batch_size=32):
    # Define the transformations
    transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.1307,), (0.3081,))  # MNIST mean and std
    ])

    # Load MNIST dataset
    dataset = datasets.MNIST(
        root='../data', 
        train=train,
        download=True,
        transform=transform
    )

    # Create dataloader
    loader = DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=train,
        num_workers=2
    )

    return loader
