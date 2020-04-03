#include "../Clock.hpp"
#include <iostream>
#include <bitset>
#include <math.h>
#include <string>
#include <fstream>

constexpr unsigned int NUCLEOTIDES_PER_BLOCK = sizeof(unsigned long)*8 / 2;

unsigned char nucleotideCode(char nuc){
	if (nuc == 'G')
		return 0;
	else if (nuc == 'C')
		return 1;
	else if (nuc == 'T')
		return 2;
	else if (nuc == 'A')
		return 3;
	return -1;
}

//packs nucleotide strings into blocks of 64 bits
unsigned long *bitPack(const std::string &genome, unsigned long numBlocks) {

	unsigned long *result = (unsigned long*)calloc(numBlocks, sizeof(unsigned long));

	unsigned long i = 0;
	for (unsigned long block = 0; block < numBlocks; ++block){
		unsigned long nextBlock = 0;

		for (unsigned char j = 0; j < NUCLEOTIDES_PER_BLOCK; ++j){
			nextBlock <<= 2;

			if (i < genome.size())
				nextBlock |= nucleotideCode(genome[i]);

			++i;
		}
		result[block] = nextBlock;
	}
	return result;
}

int *findMatches(unsigned long *packedGenome, unsigned long numBlocks, unsigned long *packedStringToFind, unsigned int lengthOfStringToFind, unsigned long genomeLength){
	unsigned long positionInGenome = 0;
	for (int i = 0; i < numBlocks; ++i){
		std::bitset < sizeof(unsigned long) * 8 > blockToFind = std::bitset < sizeof(unsigned long) * 8 > (packedStringToFind[0]);
		std::bitset < sizeof(unsigned long) * 8 > currentBlock = std::bitset < sizeof(unsigned long) * 8 > (packedGenome[i]);
		std::bitset < sizeof(unsigned long) * 8 > nextBlock = std::bitset < sizeof(unsigned long) * 8 > (packedGenome[i+1]);

		//finds match positions, blocks are 64 bits each and moves in descending order to look at most signficiant bit first
		for (int j = 0;  j < 64; j = j+2){
			std::bitset <sizeof(unsigned long) * 8> tempBlock;
				unsigned long startPos = j;
				unsigned int bitCounter = 63; //use to add bits to bitset
				for(int k = startPos; k <= startPos + 63; ++k) {
					//checks to see if at end of block, if so execute else block
					if (k < 64){
						if ((63-bitCounter) < lengthOfStringToFind){
							tempBlock[bitCounter] = currentBlock[64-(k+1)];
						}
						else {
							tempBlock[bitCounter] = 0;
						}
					}
					else{
						if ((63-bitCounter) < lengthOfStringToFind){
							tempBlock[bitCounter] = nextBlock[64-(k-63)];
						}
						else {
							tempBlock[bitCounter] = 0;
						}
					}
					--bitCounter;
				}

				if ((blockToFind == tempBlock) && (positionInGenome < genomeLength) && ((genomeLength - positionInGenome) >= lengthOfStringToFind))
					std::cout << (positionInGenome/2) + 1 << " ";

				positionInGenome += 2;
			}
	}
	return 0;
}

int main(int argc, char**argv) {
	std::string genome;
	std::ifstream genomeFile(argv[1]);
	std::ifstream stringFile(argv[2]);

	char base;
	while (genomeFile >> base){
		genome += base;
	}

	unsigned long genomeLength = genome.length() * 2;
	unsigned long numBlocks = genome.size() / NUCLEOTIDES_PER_BLOCK;
	if (numBlocks * NUCLEOTIDES_PER_BLOCK != genome.size())
		++numBlocks;

	Clock c;
	unsigned long *packed = bitPack(genome, numBlocks);
	std::string stringToFind;
	while (std::getline(stringFile, stringToFind)){
		unsigned int lengthOfStringToFind = stringToFind.length() * 2;
		unsigned long *packedStringToFind = bitPack(stringToFind, numBlocks);
		int *matches = findMatches(packed, numBlocks, packedStringToFind, lengthOfStringToFind, genomeLength);
		std::cout << std::endl;
	}
	c.ptock();

	return 0;
}
