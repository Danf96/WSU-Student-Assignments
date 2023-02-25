import sys
from rankings import *

def main():
    test = TF_IDF("./wine.csv")
    print(test.tf(2598, 'lasdkfj'))


if __name__ == "__main__":
    main()
