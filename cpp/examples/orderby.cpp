#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;



int main(int argc, char** argv){
	if ( argc < 3 ){
		cerr << argv[0] << " <File> <Field>\n";
		return 1;
	}
	TiObj obj(false,argv[1]);
	obj->orderby("name");
	cout << obj << endl;
	return 0;
}
