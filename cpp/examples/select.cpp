#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;



int main(int argc, char** argv){
	if ( argc < 3 ){
		cerr << argv[0] << " <File> <Class>\n";
		return 1;
	}
	TiObj obj(false,argv[1]);
	TiObj filtered = obj->select(argv[2]);
	cout << filtered << endl;
	return 0;
}
