import sys

def main():
    """Reads in string from standard input, converts to tuple, then reduces similar k,v pairs through a dictionary"""
    input_dict = {}
    for line in sys.stdin:
        my_tuple: tuple[str, tuple[int,int]] = eval(line)
        if my_tuple[0] not in input_dict:
            input_dict[my_tuple[0]] = my_tuple[1]
        else:
            input_dict[my_tuple[0]] = (input_dict[my_tuple[0]][0] + my_tuple[1][0], input_dict[my_tuple[0]][1] + my_tuple[1][1])
    for tup in input_dict.items():
        print(f'{tup[0]}{{size}}  {tup[1][1]}')
        print(f'{tup[0]}{{visits}}  {tup[1][0]}')

if __name__ == '__main__':
    main()
