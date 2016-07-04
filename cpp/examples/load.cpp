#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;


int main(int argc, char** argv){
	try {
		TiObj text("name=felipe;idade=28;score=123.123 ");
		//cout << text << endl;

		TiObj file1;
		file1->load( argv[1] );

		for ( auto var : file1->var ){
			cout << var->name << " -> " << var->Str() << endl;
		}

		for ( auto obj : file1->box ){
			cout << obj << endl;
		}


	} catch (tiexception e){
		cout << e.what() << endl;
	}

	return 0;
}
