import os, sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--dir", help="directory to rename files in")
parser.add_argument("--init_counter", help="number to start files at")
args = parser.parse_args()

path = os.getcwd()+"/"+args.dir
files = os.listdir(path)
if args.init_counter is not None:
    counter = args.init_counter
else:
    counter = 0

# files = files[:10]

for file in files:
    if file[-4:] == '.jpg':
        os.rename(path+"/"+file, f"{path}/tr_{counter}.jpg")
        counter += 1
print("done!")