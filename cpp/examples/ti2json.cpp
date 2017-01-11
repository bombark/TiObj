#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;



int main(int argc, char** argv){
	if ( argc < 2 ){
		return 1;
	}

	TiObj file1;
	file1->load( argv[1] );
	cout << file1->toJson();

	return 0;
}
