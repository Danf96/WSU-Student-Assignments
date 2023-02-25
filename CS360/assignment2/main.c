#include <stdio.h>
#include "lineNum.h"

int main(){
	char * wordtosearch = "aardvarkara";
	char * word2 = "horse";
	char * word3 = "mellow";
	char * word4 = "fi sh";
	char * word5 = "oliphant";
	char * word8 = "beta";
	char * dictionary = "tiny_9";
	int line;
	line = lineNum(dictionary, wordtosearch, 9);
	printf("aardvark is %d\n", line);
	line = lineNum(dictionary, word2, 9);
	printf("horse is %d\n", line);
	line = lineNum(dictionary, word3, 9);
	printf("mellow is %d\n", line);
	line = lineNum(dictionary, word4, 9);
	printf("fi sh is %d\n", line);
	line = lineNum(dictionary, word5, 9);
	printf("oliphant is %d\n", line);
	line = lineNum(dictionary, word8, 9);
	printf("beta is %d\n", line);
	return 0;
}
