import random
import string
def main():
    validchars = [',', 'o', '=', '.']
    for _ in range(8):
        sample = random.choices(validchars, k=20)
        print(" ".join(sample))


if __name__ == '__main__':
    main()
