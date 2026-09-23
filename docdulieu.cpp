#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int main(){
	vector<std::string> wordList;
	ifstream mofile("words.txt");
	if (!mofile.is_open()) {
		cerr << "Loi: Khong tim thay file du lieu!" <<std::endl;
		return 0;
	}
	string danhsach;
	while(std::getline(mofile, danhsach)){
		wordList.push_back(danhsach);
	}
	mofile.close();
	
	return 0;
}
