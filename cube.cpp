#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "cube.h"

std::unordered_set<std::string> words;
struct Cubbie cubbies[64];
std::unordered_map<char, std::vector<int>> letterMap;
//Cubbie struct:
//char sym
//int neighb[26]
//int used?
//int visited?

//cube structs:
//  64 element array of Cubbie structs
//  map of (char letter, int occurences[64]) 

//words: read in as set

//DONE:
//first try at algorithm
//environment setup
//read in files
//create data structures

//TODO:
//make neighbor mapping
//dfs on words
//multithread
//write real makefile
//cleanup/make more c++y
int copyNeighbs(int sheet, int cube, int offset) {
	int i;
	for (i = 0; i < 8 && cub[cube%16][i] >= 0; i++) {
		cubbies[cube].neighb[i+offset] = cub[cube%16][i] + (sheet*16);
	}
	return i;
}


void SetNeighbors() {
	//sorry this feels super hacky but it was fast to implement 
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 16; j++) {
			int offset = copyNeighbs(i,16*i+j,0);
			switch(i) {
				case 0:
					cubbies[j].neighb[offset] = 16+j;
					copyNeighbs(1,j,offset+1);
					break;
				case 1:
				case 2:
					cubbies[16*i+j].neighb[offset] = 16*(i+1)+j;
					cubbies[16*i+j].neighb[offset+1] = 16*(i-1)+j;
					copyNeighbs(i+1, 16*i+j, offset+2);
					copyNeighbs(i-1, 16*i+j, offset*2+2);
					break;
				case 3:
					cubbies[48+j].neighb[offset] = 32+j;
					copyNeighbs(2,48+j,offset+1);
					break;
				default:
					std::cout << "We've got problems...";
			}
		}
	}
	for (int i = 0; i < 64; i++) {
		std::cout << i << ": ";
		for (int j = 0; j<26; j++) {
			std::cout << cubbies[i].neighb[j] << " ";
		}
		std::cout << std::endl;
	}
}

int ReadCube(FILE *f_c) {
	char letters[65];
	if (fscanf(f_c, "%s", letters) == EOF) {
		//done!
		return 1;
	}
	for (int i = 0; i < 64; i++) {
		cubbies[i].letter = letters[i];
		letterMap[letters[i]].push_back(i);
	}
	

	//populate structs:
	//  update letter in CubbieStruct array x
	//  add to list of neighbors in map x

	return 0;
}

int main(int argc, char *argv[]) {
	int i;
	
	//init cube struct: make neighbor list: only needs to be done once
	//new function: read in cube file: will have to do multiple times, change sym, init used/visited=0
	// read in word-file
	SetNeighbors();
	FILE *f_w = fopen(argv[2], "r");
	if (f_w == NULL) {
		std::cout << "unable to open word-file\n";
		return -1;
	}
	
	char word[50];
	while(fscanf(f_w, "%s", word) != EOF) {
		bool punc = false;
		for (i=0; i < 50 && word[i] != '\0'; i++) {
			if (isalpha(word[i])) {
				char c = word[i];
				word[i] = tolower(c);
			} else {
				//word contains punctuation/numeric character: do not add
				punc = true;
				break;
			}
		}	
		if (!punc) {
			//add word to set
			words.insert(word);
		}
	}
	fclose(f_w);

	FILE *f_c = fopen(argv[1], "r");
	if (f_c == NULL) {
		std::cout << "unable to open cube-file\n";
		return -1;
	}
	while (ReadCube(f_c) == 0) {

		//do stuff
	}


	//read in cube (def should be own func)


	return 0;
}

	/*std::unordered_set<std::string>::iterator itr;
	for (itr = words.begin(); itr != words.end(); itr++)
		std::cout << (*itr) << std::endl;*/

/*std::unordered_map<char, std::vector<int>>::iterator itr;
	std::vector<int>::iterator itr2;
	for (itr = letterMap.begin(); itr != letterMap.end(); itr++) {
		std::cout << itr->first << ": ";
		for (itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++)
			std::cout << (*itr2) << " ";
	}
	std::cout << std::endl;*/