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
			danhsach2.pop_back();
		}
		if(!danhsach2.empty()){
			DStu.push_back(danhsach2);
		}
	}
	file.close();
	return DStu;
}
void nhaptay(vector<string>& wordList, const string& TuMoi){
	if(TuMoi.empty()) return;
	wordList.push_back(TuMoi);
	ofstream danhsach3("words.txt", ios::app);
	if(danhsach3.is_open()){
		danhsach3<<TuMoi<<endl;
		danhsach3.close();
		cout<<TuMoi<<endl;
	}
	else{
		cout<<"Mission faild"<<endl;
	}
}
void UpFile(vector<string>& wordList, const string& TVfile){
	ifstream FileTV(TVfile);
	if(!FileTV.is_open()){
		cout<<"Loi: UP that bai"<<TVfile<<endl;
		return;
	}
	ofstream danhsach4("words.txt", ios::app);
	string voca;
	int sotu = 0;
	while (getline(FileTV, voca)){
		if(!voca.empty() && voca.back() == '\r'){
			voca.pop_back();
		}
		if(!voca.empty()){
			wordList.push_back(voca);
			if(danhsach4.is_open()){
				danhsach4<<voca<<endl;
			}
			sotu++;
		}
	}
	FileTV.close();
	danhsach4.close();
	cout<<sotu<<TVfile<<endl;
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
