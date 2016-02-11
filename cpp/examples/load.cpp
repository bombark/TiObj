#include <iostream>
#include <string.h>
#include "../include/tiobj.hpp"

using namespace std;




int main(int argc, char** argv){
	TiObj obj;
	obj.loadFile("teste.ti");
	cout << obj;
	return 0;
}


