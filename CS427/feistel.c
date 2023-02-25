//Daniel Fuchs
//Written in C
//CS 427
//DANI; 4 1 14 9; 11 3 47 29
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//NOTE: Example primes are 19, 67, 2, 47
unsigned int namePrimes[] = {11, 3, 47, 29};
unsigned int strToInt(char * input);
int pairCheck(char * input, int mod);
unsigned int padBlock(char * input, unsigned int hex);
unsigned int rotateLeft(unsigned int original, int n);
unsigned int lowerByte(unsigned int original);
unsigned int upperByte(unsigned int original);
unsigned int encFunc(unsigned int n, unsigned int message);
unsigned int feistelLoop(unsigned int counter, unsigned int * subKey);

int main (int argc, char *argv[]){
	char inputChars[256];
	char nonceChars[5];
	char keyChars[5];
	char blockChars[5];
	unsigned int subKey[4];
	memset(blockChars, '\0', 5);

	scanf("%4s %4s %255s", nonceChars, keyChars, inputChars); 			//take in input from user
	if (pairCheck(nonceChars, 2) || pairCheck(keyChars, 2) || pairCheck(inputChars, 2)){
		fprintf(stderr, "Error: all inputs must be in pairs\n");
		exit(1);
	}
	unsigned int nonceInt = strToInt(nonceChars);
	unsigned int keyInt = strToInt(keyChars);

	int numBlocks = 0;
	
	int inputLeft = strlen(inputChars);
	while(inputLeft >= 4){ 		//counts how many blocks we have to correctly allocate amount of blocks we need in our int array
		inputLeft -= 4;
		numBlocks++;

	}
	if (inputLeft > 0){ 		//checking in case there is a last block that must be padded
		numBlocks++;
	}
	unsigned int blockInt[numBlocks]; 		//dynamically declares array of unsigned ints to store our blocks we will xor with the encrypted counter
	
	for (int i = 0, j = 0; j < strlen(inputChars); i++, j += 4){
		strncpy(blockChars, inputChars + j, 4); 	//the idea here is to copy 4 characters from the input string at a time, 
													//with the offset increasing by 4 with every strncpy to ensure the correct amount is being counted
		blockInt[i] = strToInt(blockChars);
	}
	if (inputLeft > 0){
		strncpy(blockChars, inputChars + (strlen(inputChars) - 2), 2); //we know the input is already in pairs and the last unfinished block will be 2 elements
		blockInt[numBlocks-1] = padBlock(blockChars, strToInt(blockChars));
	}
	nonceInt = padBlock(nonceChars, nonceInt);
	keyInt = padBlock(keyChars, keyInt);
	
	

	for(int n = 0; n < 4; n++){ 							//generates all the subkeys we need
		subKey[n] = lowerByte(rotateLeft(keyInt, n*4)) ^ lowerByte(namePrimes[n]);

	}
	unsigned int counter = nonceInt; //unnecesary, can refactor
	for(int i = 0; i < numBlocks; i++){ 					//must now incorporate counter+nonce with feistel loop
		blockInt[i] = blockInt[i] ^ feistelLoop(counter, subKey);
		counter = (counter + 1) & 0xFFFF;
	}
	
	for(int i = 0; i < numBlocks; i++){ 					//prints out every block stored in the int array with proper padding
		printf("%0*x", 4, blockInt[i]);
	}
	printf("\n");
}

unsigned int strToInt(char * input){ 						//converts character array into a base 16 number
	char * endptr;
	return (unsigned int)strtol(input, &endptr, 16);
}

int pairCheck(char * input, int mod){ 						//ensures all inputs are in pairs returns 1 if not all inputs are even, also used to ensure blocks are correctly padded
	return(!(strlen(input) % mod == 0));
}
unsigned int padBlock(char * input, unsigned int hex){ 			//pads blocks that are are empty (ab becomes ab00)
	if(pairCheck(input, 4)){
		hex <<= (unsigned int)8; 						//adds 2 more hex numbers for padding
	}
	return hex;
}
unsigned int rotateLeft(unsigned int original, int n){ 		//note: only done with blocks of 16 bits (4 hex nums) in mind
	unsigned int upperHalf, lowerHalf;
	upperHalf = original >> (unsigned int)(16 - n);
	lowerHalf = (original << (unsigned int)n) & (unsigned int)0xFFFF;
	return (upperHalf | lowerHalf);
}
unsigned int lowerByte(unsigned int original){
	return (original & (unsigned int)0xFF);
}
unsigned int upperByte(unsigned int original){
	return (original & (unsigned int)0xFF00);
}
unsigned int feistelLoop(unsigned int counter, unsigned int * subKey){ 	//will implement 4 rounds in here and will be applied to each block
	unsigned int left, right, temp;
	left = upperByte(counter) >> 8;
	right = lowerByte(counter);
	for(int i = 0; i < 4; i++){ 							//will go through 4 rounds of feistel encryption
		temp = encFunc(subKey[i], right);
		temp = left ^ temp;
		left = right;
		right = temp;
	}
	temp = right; 											//final switch at end of feistel
	right = left;
	left = temp << 8;
	return (left | right); 									//returns combination of left and right side

}

unsigned int encFunc(unsigned int n, unsigned int message){ //our F function is a simple xor
	return (n ^ message);
}