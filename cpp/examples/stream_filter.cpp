#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;

int main(int argc, char** argv){
	TiObj obj;
	TiStream stream(stdin);

	if ( argc > 1 ){
		while ( stream.next(obj) ){
			if ( obj->is(argv[1]) )
				cout << obj->encode(1) << endl;
		}
	} else {
		while ( stream.next(obj) ){
			cout << obj->encode(1) << endl;
		}
	}
	return 0;
}
