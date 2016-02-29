#include <iostream>
#include <string.h>
#include "../include/tiobj.hpp"

using namespace std;




int main(int argc, char** argv){
	TiObj obj;
	obj.loadFile("teste.ti");

//	cout << obj;
	string name = obj.atStr("name");
	cout << name << endl;
	
	
	for (int i=0; i<obj.size(); i++){
		cout << obj.box[i].atStr("name") << endl;
	}


	return 0;
}


