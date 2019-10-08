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
struct Cubbie cubbies[64];
std::unordered_map<char, std::vector<int>> letterMap;
int wordCount = 0;

int copyNeighbs(int sheet, int cube) {
	int i;
	for (i = 0; i < 8 && cub[cube%16][i] >= 0; i++) {
		cubbies[cube].neighb.push_back(cub[cube%16][i] + (sheet*16));
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
					cubbies[j].neighb.push_back(16+j);
					copyNeighbs(1,j);
					break;
				case 1:
				case 2:
					cubbies[16*i+j].neighb.push_back(16*(i+1)+j);
					cubbies[16*i+j].neighb.push_back(16*(i-1)+j);
					copyNeighbs(i+1, 16*i+j);
					copyNeighbs(i-1, 16*i+j);
					break;
				case 3:
					cubbies[48+j].neighb.push_back(32+j);
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
		for (int j = 0; j<cubbies[i].neighb.size(); j++) {
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
			//std::cout << cubbies[(*p).back()].neighb.size() << std::endl;
			int neighbSize = cubbies[(*p).back()].neighb.size();
			for (int n = 0; n < neighbSize; n++) {
				for (int i = 0; i < (*p).size(); i++)
					std::cout << (*p)[i] << " ";
				std::cout << std::endl;
				//check each neighbor of the last node of the path 
				int ni = cubbies[(*p).back()].neighb[n];
				if (cubbies[ni].letter = word[w]) {
					//check if this cubbie has already been used in this path
					std::vector<int>::iterator it = std::find((*p).begin(), (*p).end(), ni);
					if (it == (*p).end()) {
						std::vector<int> newPath(*p);
						//add new cubbies to back of path
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
	std::vector<int> used;
	std::vector<int> tempVec;
	try {
		tempVec = (letterMap.at(word[0]));
	}
	catch(const std::out_of_range &e) {
		//first letter of word does not exist in cube
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
		paths.pop();
		if (cubbies[std::get<0>(node)].letter == word[std::get<1>(node)]) {
			bool alreadyUsed = false;
			for (int i = 0; i <= std::get<1>(node)-1; i++) {
				if (used[i] == std::get<0>(node)) {
					alreadyUsed = true;
				}
			}
			//std::vector<int>::iterator it = std::find(used.begin(), used[std::get<1>(node)], std::get<0>(node));
			if (!alreadyUsed) {
				if (std::get<1>(node) == word.size()-1){
					wordCount++;
					return;
				}
				used.push_back(std::get<0>(node));
				for (int i = 0; i < cubbies[std::get<0>(node)].neighb.size(); i++) {
					std::tuple<int,int> newTup = std::make_tuple(cubbies[std::get<0>(node)].neighb[i], std::get<1>(node)+1);
					paths.push(newTup);
					
				}
			}
		} 
	}
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

	while (ReadCube(f_c) == 0){
	//ReadCube(f_c);
		wordCount = 0;
		std::unordered_set<std::string>::iterator itr;
		for(itr = words.begin(); itr != words.end(); itr++) {
			for (int i = 0; i < 64; i++) {
				cubbies[i].used = false;
			}
			DFS2(*itr);
		}
		std::cout << wordCount << std::endl;
	}
	fclose(f_c);
	


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