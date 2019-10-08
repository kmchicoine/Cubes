#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>
#include "cube.h"

std::unordered_set<std::string> words;
struct Cubbie cubbies[64];
std::unordered_map<char, std::vector<int>> letterMap;
int wordCount = 0;
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
//make neighbor mapping

//TODO:
//dfs on words
//test against example
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
	//sorry this feels super hacky but my brain won't brain better
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
	/*
	TODO: make vectors?
	for (int i = 0; i < 64; i++) {
		std::cout << i << ": ";
		for (int j = 0; j<26; j++) {
			std::cout << cubbies[i].neighb[j] << " ";
		}
		std::cout << std::endl;
	}*/
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
	return 0;
}

int DFS(std::string word) {
	//S is a stack
	//push all cubes in letterMap.second to S
	//while S is not empty:
	//	v = S.pop
	//	if v.index == word.size()-1
	//		word is in cube
	//	if v is not used 
	//		v = used
	//		for all n in cubbies[v].neighb
	//			if n.letter = word[v.index+1] 
	//				n.index = v.index+1
	//				S.push(n)
	std::stack<int> s;
	std::vector<int>::iterator itr;
	std::vector<int> tempVec;
	try {
		letterMap.at(word[0]);
	}
	catch(const std::out_of_range &e) {
		//first letter of word does not exist in cube
		return -1;
	}
	for (itr = tempVec.begin(); itr != tempVec.end(); itr++) {
		cubbies[*itr].index = *itr;
		s.push(*itr);
	}
	while(!s.empty()) {
		int v = s.top();
	    s.pop();
	    if (cubbies[v].index == word.size()-1) {
	    	//word has been found in cube
	    	wordCount++;
	    	//reset used values?
	    	return 0;
	    }
	    if (cubbies[v].used == false) {
	    	cubbies[v].used = true;
	    	for (int i = 0; i < 26 && cubbies[v].neighb[i] != -1; i++) {
	    		int n = cubbies[v].neighb[i];
	    		if (cubbies[n].letter == word[cubbies[v].index+1]) {
	    			cubbies[n].index = cubbies[v].index+1;
	    			s.push(n);
	    		} 
	    	}
	    }
	}

	/*if (letterIndex == word.size()-1) {
		//the word is in the cube
		wordCount++;
		return;
	} else if (cubbies[cube].letter != word[letterIndex]) {
		//this path does not work
		return;
	} else {
		//this is the letter we want and it is unused:
		//check all of its neighbors
		for (int i = 0; i < 26 && cubbies[cube].neighb[i] >= 0; i++) {
			DFS(word, letterIndex+1; cubbies[cube].neighb[i]);
		}
	}*/

}

int main(int argc, char *argv[]) {
	int i;
	//set the list of neighbors for each cubbie
	SetNeighbors();

	// read in word-file
	FILE *f_w = fopen(argv[2], "r");
	if (f_w == NULL) {
		std::cout << "unable to open word-file\n";
		return -1;
	}
	
	char tempWord[50];
	while(fscanf(f_w, "%s", tempWord) != EOF) {
		bool punc = false;
		for (i=0; i < 50 && tempWord[i] != '\0'; i++) {
			if (isalpha(tempWord[i])) {
				char c = tempWord[i];
				tempWord[i] = tolower(c);
			} else {
				//word contains punctuation/numeric character: do not add
				punc = true;
				break;
			}
		}	
		if (!punc) {
			//add word to set
			words.insert(tempWord);
		}
	}
	fclose(f_w);

	//read in cube-file (as appropriate)
	FILE *f_c = fopen(argv[1], "r");
	if (f_c == NULL) {
		std::cout << "unable to open cube-file\n";
		return -1;
	}
	/*while (ReadCube(f_c) == 0) {
		std::unordered_set<std::string>::iterator itr;
		itr = words.begin();
		//for (itr = words.begin(); itr != words.end(); itr++)
		//	std::cout << (*itr) << std::endl;
		DFS(*itr);
		//dfs on words
		//for each node in letterMap[starting letter].second
		//  DFS(word, 0, node)
	}*/
	ReadCube(f_c);
	fclose(f_c);
	std::unordered_set<std::string>::iterator itr;
	for(itr = words.begin(); itr != words.end(); itr++) {
		DFS(*itr);
	}
	std::cout << wordCount << std::endl;
	
	


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