#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;


int main(int argc, char** argv){
	TiObj b(false, "../tests/all.ti");
	//cout << b.toAsm();
	cout << b;
	return 0;
}
