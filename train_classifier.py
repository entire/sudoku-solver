from digit_classifier.models.sudoku_net import SudokuNet
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.datasets import mnist
from sklearn.preprocessing import LabelBinarizer
from sklearn.metrics import classification_report
import argparse

ap = argparse.ArgumentParser()
ap.add_argument("-m", "--model", required=True, help="path to model")
args = vars(ap.parse_args())

# initial learning rate
INIT_LEARNING_RATE = 1e-3
# num of epochs to train
EPOCHS = 10
BATCH_SIZE = 128

# split into training and test data
((trainData, trainLabels), (testData, testLabels)) = mnist.load_data()

# add a channel dim to the digits to indicate that they are grayscale
trainData = trainData.reshape((trainData.shape[0], 28, 28, 1))
testData = testData.reshape((testData.shape[0], 28, 28, 1))

# add scale data to range of [0,1]
trainData = trainData.astype("float32") / 255.0
testData = testData.astype("float32") / 255.0

# convert labels from int to vectors
le = LabelBinarizer()
trainLabels = le.fit_transform(trainLabels)
testLabels = le.transform(testLabels)

# init optimizer and model
opt = Adam(lr=INIT_LEARNING_RATE)
model = SudokuNet.build(w=28, h=28, d=1, c=10)
model.compile(loss="categorical_crossentropy", optimizer=opt, metrics=["accuracy"])

# train the model
H = model.fit(
    trainData, trainLabels,
    validation_data=(testData, testLabels),
    batch_size=BATCH_SIZE,
    epochs=EPOCHS,
    verbose=1
)

# evaluate network
pred = model.predict(testData)
print(
    classification_report(
        testLabels.argmax(axis=1),
        pred.argmax(axis=1),
        target_names=[str(x) for x in le.classes_])
    )
model.save(args["model"], save_format="h5")
