from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D
from tensorflow.keras.layers import MaxPooling2D
from tensorflow.keras.layers import Activation
from tensorflow.keras.layers import Flatten
from tensorflow.keras.layers import Dense
from tensorflow.keras.layers import Dropout


class SudokuNet:
    @staticmethod
    def build(w, h, d, c):
        """
        configures the sudokunet class

        w: width of the MNIST digit (28 pixels)
        # h: height of the MNIST digit (28 pixels)
        # d: channels of the MNIST digit image (1 grayscale channel)
        # c: number of digits (0-9)
        """

        # initialize the model
        model = Sequential()
        input_shape = (h, w, d)
        
        # First: CONV => RELU => POOL layers
        model.add(Conv2D(32, (5,5), padding="same",input_shape=input_shape))
        model.add(Activation("relu"))
        model.add(MaxPooling2D(pool_size=(2,2)))

        # Second: CONV => RELU => POOL layers
        model.add(Conv2D(32, (3,3), padding="same"))
        model.add(Activation("relu"))
        model.add(MaxPooling2D(pool_size=(2,2)))

        # first set of FC => RELU layers
        model.add(Flatten())
        model.add(Dense(64))
        model.add(Activation("relu"))
        model.add(Dropout(0.5)) # 50% dropout

        # second set of FC => RELU layers
        model.add(Dense(64))
        model.add(Activation("relu"))
        model.add(Dropout(0.5)) # 50% dropout

        # softmax classifier
        model.add(Dense(c))
        model.add(Activation("softmax"))
        return model


