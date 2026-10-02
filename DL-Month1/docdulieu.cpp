#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

int main(){
	ifstream FileTopic("topic.json");
	if (!FileTopic.is_open()) {
		cout<<"Loi: khong tim thay file du lieu!"<<endl;
		return 0;
	}
	json ChuDe;
	FileTopic >> ChuDe;
	FileTopic.close();
	string tenchuDe = ChuDe["topic"];
	cout<<"Ten chu de: "<<tenchuDe<<endl;
	ifstream FileData("data.json");
	if(!FileData.is_open()) {
		cout<<"Loi: khong tim thay file du lieu!"<<endl;
		return 0;
	}
	json DuLieu;
	FileData >> DuLieu;
	FileData.close();
	if(DuLieu.contains(tenchuDe)){
		vector<string> DanhSachTu = DuLieu[tenchuDe];
		cout<<DanhSachTu.size()<< tenchuDe <<endl;
		for(string tu : DanhSachTu){
			cout<< tu <<endl;
		}
	}
	else{
		cout<<"Khong tim thay chu de trong file du lieu!"<<endl;
	}
	return 0;
}