#include <iostream>
#include "../include/tiparser.hpp"
#include "../include/tiobj.hpp"

using namespace std;



int main(int argc, char** argv){
	if ( argc < 2 ){
		cerr << "Syntax: " << argv[0] << " command\n";
	}

	string cmd = argv[1];
	for (size_t i=2; i<argc; i++){
		cmd += " ";
		cmd += argv[i];
	}

	/*TiObj obj;
	TiStream stream(stdin);
	while ( stream.next(obj) ){
		cout << obj;
	
		/*for (size_t i=0; i<obj.length(); i++){
			TiVar& var = obj.var(i);
			setenv( var.name.c_str(), var.Str().c_str(), true );
		}
		system( cmd.c_str() );*/
	//}
	return 0;
}
