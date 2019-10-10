#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>
#include <list>
#include <algorithm>
#include <tuple>
#include "cube.h"

std::unordered_set<std::string> words;
struct Cubie cubies[64];
std::unordered_map<char, std::vector<int>> letterMap;
int wordCount = 0;

int copyNeighbs(int sheet, int cube) {
	int i;
	for (i = 0; i < 8 && cub[cube%16][i] >= 0; i++) {
		cubies[cube].neighb.push_back(cub[cube%16][i] + (sheet*16));
	}
	return i;
}


void SetNeighbors() {
	//sorry this feels super hacky but my brain won't brain better
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 16; j++) {
			copyNeighbs(i,16*i+j);
			switch(i) {
				case 0:
					cubies[j].neighb.push_back(16+j);
					copyNeighbs(1,j);
					break;
				case 1:
				case 2:
					cubies[16*i+j].neighb.push_back(16*(i+1)+j);
					cubies[16*i+j].neighb.push_back(16*(i-1)+j);
					copyNeighbs(i+1, 16*i+j);
					copyNeighbs(i-1, 16*i+j);
					break;
				case 3:
					cubies[48+j].neighb.push_back(32+j);
					copyNeighbs(2,48+j);
					break;
				default:
					std::cout << "We've got problems...";
			}
		}
	}
	/*
		for (int i = 0; i < 64; i++) {
		std::cout << i << ": ";
		for (int j = 0; j<cubies[i].neighb.size(); j++) {
			std::cout << cubies[i].neighb[j] << " ";
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
		cubies[i].letter = letters[i];
		letterMap[letters[i]].push_back(i);
	}
	return 0;
}

void BFS(std::string word) {
	std::list<std::vector<int>> paths;
	
	std::vector<int> tempVec;
	try {
		tempVec = (letterMap.at(word[0]));
	}
	catch(const std::out_of_range &e) {
		//first letter of word does not exist in cube
		return;
	}
	std::cout << "tempVec: ";
	for (int i = 0; i < tempVec.size(); i++)
			std::cout << tempVec[i] << " ";
		std::cout << std::endl;
	std::vector<int>::iterator itr;
	for (itr = tempVec.begin(); itr != tempVec.end(); itr++) {
		//push all first letters into paths
		std::vector<int> first(*itr);
		paths.push_front(first);
	}
	std::cout << word << std::endl;
	for (int w = 1; w < word.size(); w++) {
		std::list<std::vector<int>>::iterator p;
		for (auto p = paths.begin(); p != paths.end(); ) {
			//number of paths could change with every iteration of w
			//std::cout << cubies[(*p).back()].neighb.size() << std::endl;
			int neighbSize = cubies[(*p).back()].neighb.size();
			for (int n = 0; n < neighbSize; n++) {
				for (int i = 0; i < (*p).size(); i++)
					std::cout << (*p)[i] << " ";
				std::cout << std::endl;
				//check each neighbor of the last node of the path 
				int ni = cubies[(*p).back()].neighb[n];
				if (cubies[ni].letter = word[w]) {
					//check if this cubbie has already been used in this path
					std::vector<int>::iterator it = std::find((*p).begin(), (*p).end(), ni);
					if (it == (*p).end()) {
						std::vector<int> newPath(*p);
						//add new cubies to back of path
						newPath.push_back(ni);
						//add new paths to the front of list
						paths.push_front(newPath);
					}
				}
			}
			//old path is too short now and has been replaced (maybe) by new paths: delete p
			//erase returns iterator to next element: this is how we advance p
			p = paths.erase(p);
		}
		std::cout << "finished pi " << std::endl;

	}
	std::cout << "ending" << std::endl;
	if (!paths.empty()) {
		std::cout << word << " found\n";
		wordCount++;
	}
}

void DFS2(std::string word) {
	std::stack<std::tuple<int, int>> paths;
	int used[word.size()];
	std::vector<int> tempVec;
	try {
		tempVec = (letterMap.at(word[0]));
	}
	catch(const std::out_of_range &e) {
		//first letter of word does not exist in cube
		return;
	}

	if (word.size() == 1) {
		//word is only one letter, and that letter exists in cube
		wordCount++;
		return;
	}

	std::vector<int>::iterator itr;
	for (itr = tempVec.begin(); itr != tempVec.end(); itr++) {
		//push all first letters into paths
		std::tuple<int, int> first(*itr, 0);
		paths.push(first);
	}
	while (!paths.empty()) {
		std::tuple<int,int> node = paths.top();
		int loc = std::get<0>(node);
		int let = std::get<1>(node);
		paths.pop();	
		
		used[let] = loc;
		//check all neighbors of this (correct) letter:
		//if any neighbor is a valid option(correct letter
		//and not yet used), push to paths
		for (int i = 0; i < cubies[loc].neighb.size(); i++) {
			int curN = cubies[loc].neighb[i];
			if (cubies[curN].letter == word[let+1]) {
				bool alreadyUsed = false;
				for (int i = 0; i < let+1; i++) {
					if (used[i] == curN) {
						alreadyUsed = true;
						break;
					}
				}
				if (!alreadyUsed) {
					if (let+1 == word.size()-1){
						//at the end of the word- found a path!
						wordCount++;
						return;
					}
					std::tuple<int,int> newTup = std::make_tuple(curN, let+1);
					paths.push(newTup);
				}
			}
		}
	}
}


int main(int argc, char *argv[]) {
	std::vector<int> output;
	output.reserve(1000);
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
		for (int i=0; i < 50 && tempWord[i] != '\0'; i++) {
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

	while (ReadCube(f_c) == 0){
		wordCount = 0;
		std::unordered_set<std::string>::iterator itr;
		for(itr = words.begin(); itr != words.end(); itr++) {
			for (int i = 0; i < 64; i++) {
				cubies[i].used = false;
			}
			DFS2(*itr);
		}
		letterMap.clear();
		output.push_back(wordCount);
		//std::cout << wordCount << std::endl;
	}
	for (int i = 0; i < output.size(); i++) {
		std::cout << output[i] << std::endl;
	}
	fclose(f_c);
	return 0;
}