import torch
import torchvision
import torch.nn as nn
from torchvision import datasets, transforms
from torchvision import transforms, models
from torch.utils.data import SubsetRandomSampler
from torch.autograd import Variable
from torch import nn, optim
import torch.nn.functional as F
from networks.sudoku_net import SudokuNet
from loader.digit_dataset_loader import DigitDatasetFromFile

BATCH_SIZE=2
LR=0.0015
EPOCHS=8

def get_transform():
    return transforms.Compose([
        transforms.Resize((28, 28)),
        transforms.Grayscale(num_output_channels=1),
        transforms.ToTensor(),
        transforms.Normalize((0.1307,), (0.3081,))
    ])

def run_test(loader):
    examples = enumerate(loader)
    # batch_idx, (example_data, example_targets) = next(examples)
    # print(example_data.shape)

def train():
    # Instantiate our model
    model = SudokuNet()

    # Define our loss function
    loss_function = nn.NLLLoss()

    # Define the optimier
    optimizer = optim.Adam(model.parameters(), lr=LR)

    steps = 0
    print_every = 50
    train_losses, test_losses = [], []

    for e in range(EPOCHS):
        print("start")
        running_loss = 0
        print(train_loader)
        for images, labels in train_loader:
            print(images)
            steps += 1
            
            # Prevent accumulation of gradients
            optimizer.zero_grad()
            
            # forward pass
            output = model(images)
            
            # loss calculation
            loss = loss_function(output, labels)
            
            # backward pass
            loss.backward()
            
            # weight optimization
            optimizer.step()

            running_loss += loss.item()
            if steps % print_every == 0:
                test_loss = 0
                accuracy = 0

                # Turn off gradients for validation
                with torch.no_grad():
                    model.eval()
                    for images, labels in test_loader:
                        output = model(images)
                        test_loss += loss_function(output, labels)

                        ps = torch.exp(output)
                        # Get our top predictions
                        top_p, top_class = ps.topk(1, dim=1)
                        equals = top_class == labels.view(*top_class.shape)
                        accuracy += torch.mean(equals.type(torch.FloatTensor))

                model.train()

                train_losses.append(running_loss/len(train_loader))
                test_losses.append(test_loss/len(test_loader))

                print("Epoch: {}/{}.. ".format(e+1, epochs),
                    "Training Loss: {:.3f}.. ".format(train_losses[-1]),
                    "Test Loss: {:.3f}.. ".format(test_losses[-1]),
                    "Test Accuracy: {:.3f}".format(accuracy/len(test_loader)))

    # print(next(examples))

if __name__ == "__main__":
    # setup
    transform = get_transform()

    digit_train = DigitDatasetFromFile('../dataset/train/')
    digit_test = DigitDatasetFromFile('../dataset/test/')

    train_loader = torch.utils.data.DataLoader(
        dataset=digit_train, 
        batch_size=BATCH_SIZE,
    )
    test_loader = torch.utils.data.DataLoader(
        dataset=digit_test, 
        batch_size=BATCH_SIZE,
    )

    run_test(test_loader)

    import matplotlib.pyplot as plt

    # fig = plt.figure()
    # for i in range(30):
    #     plt.subplot(5,6,i+1)
    #     plt.tight_layout()
    #     data = example_data[i][0]
    #     plt.imshow(data, cmap='gray', interpolation='none')
    #     plt.title("{}".format(example_targets[i]))
    #     plt.xticks([])
    #     plt.yticks([])
    
    # train()

