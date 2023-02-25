import sys

def main():
    """Takes in string, splits, checks if true, then prints tuple with casted ints"""
    for line in sys.stdin:
        line = line.split()
        if line[3] == 'T':
            print((line[0],(int(line[1]),int(line[2]))))

if __name__ == '__main__':
    main()