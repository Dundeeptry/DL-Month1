#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

vector<string> TVTopic(const string& tenTopic){
	vector<string> DStu;
	string CD = "topics/" + tenTopic + ".txt";
	ifstream file(CD);
	if (!file.is_open()){
		cout<<"Loi: khong tim thay file du lieu!"<<tenTopic<<endl;
		return DStu;
	}
	string danhsach2;
	while (getline(file, danhsach2)){
		if(!danhsach2.empty() && danhsach2.back() == '\r'){
			DStu.pop_back();
		}
		if(!danhsach2.empty()){
			DStu.push_back(danhsach2);
		}
	}
	file.close();
	return DStu;
}
int main(){
	vector<string> wordList;
	ifstream mofile("words.txt");
	if (!mofile.is_open()) {
		cout<<"Loi: khong tim thay file du lieu!"<<std::endl;
		return 0;
	}
	string danhsach;
	while(getline(mofile, danhsach)){
		if(!danhsach.empty() && danhsach.back() == '\r'){
			danhsach.pop_back();
		}
		wordList.push_back(danhsach);
	}
	mofile.close();
	cout<<wordList.size()<<endl;
	return 0;
}
