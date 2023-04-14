//Daniel Fuchs
//Written in C
//CS 427
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>

uint16_t randPrime(); 								//gets random number, runs miller-rabin test on it 20 times and gets a new number if it fails
uint64_t strToInt(char * input); 					//converts string to 64 bit unsigned int, taken from my feistel network assignment
void verifySig(uint32_t n, char * string, uint64_t signedHash);
void signSig(char * string);
uint32_t extendedEuclid(uint32_t a, uint32_t b); 	//implementation based on Wikipedia pesudocode linked on assignment page
uint32_t eulersTotient(uint16_t p, uint16_t q); 
uint8_t millerRabinTest(uint16_t n); 				//implemented based on Wikipedia pseudocode as well as ruby example in slides
uint32_t ElfHash(const unsigned char *s); 			//implementation taken directly from Wikipedia under the PJW Hash Function page with no modifications
uint64_t fastModExpRecursive(uint64_t base, uint64_t pow, uint32_t mod); //recursively calculates fast exponentiation
int main (int argc, char *argv[]){

	srandom(time(0));
	char inputBuffer[1024];
	char * token;
	int bytesRead;
	bytesRead = read(0, inputBuffer, sizeof inputBuffer);
	token = strtok(inputBuffer, " ");
	if (!strcmp("sign", token)){ 		//do one more strtok then pass it to sign func
		token = strtok(NULL, "\"");
		token[strcspn(token, "\"")] = '\0'; //scans until next quotation mark then removes it
		signSig(token);
	}else if (!strcmp("verify", token)){ 
		char * args[3];
		args[0] = strtok(NULL, " ");
		args[1] = strtok(NULL, "\"");
		args[1][strcspn(args[1], "\"")] = '\0';
		args[2] = strtok(NULL, " ");
		uint32_t n;
		uint64_t signedHash;
		n = (uint32_t)strToInt(args[0]);
		signedHash = strToInt(args[2]);
		verifySig(n, args[1], signedHash);
	}else{
		fprintf(stderr, "invalid string format\n");
		exit(0);
	}
}
void signSig(char * string){ 	//p and q = random primes, n= modulus, t= totient
	uint16_t p = randPrime();
	uint16_t q = randPrime();
	uint32_t n = p * q;
	uint32_t e = 65537;
	uint32_t t = eulersTotient(p,q);
	printf("p = %"PRIx16 ", q = %"PRIx16 ", n = %" PRIx32 ", t = %" PRIx32 "\n", p, q, n ,t);
	printf("received message: %s.\n", string);
	uint32_t hashedStr = ElfHash(string);
	printf("message hash: %"PRIx32 "\n", hashedStr);
	//calculate d as mult inverse of e mod totient
	uint32_t privKey = extendedEuclid(e, t);
	printf("signing with the following private key: %"PRIx32 "\n", privKey);
	uint64_t signedHash = fastModExpRecursive((uint64_t)hashedStr, (uint64_t)privKey, n);
	printf("signed hash: %"PRIx64 "\n", signedHash);
	printf("uninverted message to ensure integrity: %"PRIx32 "\n", hashedStr);
	printf("complete output for verification:\n%"PRIx32 " \"%s\" %"PRIx64 "\n", n, string, signedHash);
}
uint32_t extendedEuclid(uint32_t a, uint32_t b){
	int64_t old_r, r, old_s, s, old_t, t, quotient, temp;
	old_r = a;
	r = b;
	old_s = 1;
	s = 0;
	old_t = 0;
	t = 1;
	while (r != 0){
		quotient = old_r / r;
		temp = old_r;
		old_r = r;
		r = temp - (quotient * r);
		temp = old_s;
		old_s = s;
		s = temp - (quotient * s);
		temp = old_t;
		old_t = t;
		t = temp - (quotient * t);
	}
	if(old_s < 0){ //negative modulus, get wraparound
		old_s = b + old_s;
	}
	return (uint32_t)old_s; 
}
uint32_t eulersTotient(uint16_t p, uint16_t q){
	return ((p-1)*(q-1));
}
uint16_t randPrime(){
	uint16_t p = (random() % (0xFFFF - (0x8000 + 1))) + 0x8000; //lower bound is 0x8000 , upper bound is 0xFFFF
	uint8_t i = 0;
	while(i < 20){
		if (!millerRabinTest(p)){ //if test failed
			i = 0;
			p = (random() % (0xFFFF - (0x8000 + 1))) + 0x8000;
		}else i++;
	}
	return p;
}
uint8_t millerRabinTest(uint16_t n){
	if (n % 2 == 0 || n <= 1){
		return 0; //false
	}
	uint16_t d = n - 1;
	while (d % 2 == 0){
		d = d >> 1;
	}
	uint16_t a = (random() % (n - 4)) + 3; //lower bound is 3 , upper bound is n,
	uint64_t x = fastModExpRecursive(a, d, n);
	if (x == 1 || x == n-1){
		return 1; //true
	}
	while (d != n-1){
		x = fastModExpRecursive(a, d, n);
		d *= 2;
		if (x == 1) return 0;
		if (x == n-1) return 1;
	}
	return 0;	
}

uint32_t ElfHash(const unsigned char *s)
{
    uint32_t   h = 0, high;
    while (*s)
    {
        h = (h << 4) + *s++;
        if (high = h & 0xF0000000)
            h ^= high >> 24;
        h &= ~high;
    }
    return h;
}
uint64_t fastModExpRecursive(uint64_t base, uint64_t pow, uint32_t mod){ 
	uint64_t total;
	if (pow <= 2){
		if (pow % 2 == 0){ //x^2
		return (base * base) % mod;
		}else{//x
			return(base % mod);
		}
	}else if(pow % 2 == 0){ //even
		total = fastModExpRecursive(base, (pow>>1), mod);
		return((total)*(total)) % mod;
		}else{ //odd
			total = fastModExpRecursive(base, (pow-1), mod);
			return ((total * base) % mod);
		}	
}
void verifySig(uint32_t n, char * string, uint64_t signedHash){ //hashes character string, decrypts signed hash, then compares the two
	uint32_t hashedStr = ElfHash(string);
	uint64_t hashCheck = fastModExpRecursive((uint64_t)signedHash, 65537, n);
	if ((uint64_t)hashedStr == hashCheck){
		printf("message verified!\n");
	}
	else printf("!!! message is forged !!!\n");
}
uint64_t strToInt(char * input){ 						//converts character array into a base 16 number
	char * endptr;
	return (uint64_t)strtol(input, &endptr, 16);
}

