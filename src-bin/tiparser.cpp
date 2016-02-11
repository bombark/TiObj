#include <iostream>
#include "../tiasm/tiasm.hpp"

using namespace std;


int main(int argc, char** argv){
	string out;
	if ( argc > 2 ){
		parseFile(out, argv[1]);
	} else {
		parseFileFd(out, stdin);
	}
	cout << out;
	return 0;
}
