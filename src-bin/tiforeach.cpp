#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include "tiobj.hpp"

using namespace std;


int main(int argc, char **argv){

	if ( argc < 2 ){
		cerr << "Syntax: " << argv[0] << " Command [Parameters ...]\n";
		return 1;
	}

	TiObj obj;
	obj.loadFile(stdin);
	
	string cmd;
	for (int i=0; i<obj.box.size(); i++){
		cmd = argv[1];
		cmd += " ";
		for ( int c=2; c<argc; c++ ){
			if ( argv[c][0] == '@' ){
				if ( obj.box[i].has(argv[c]+1) ){
					cmd += obj.box[i][argv[c]+1].toString();
					cmd += " ";
				}
			} else {
				cmd += argv[c];
				cmd += " ";
			}
		}
		int e = system(cmd.c_str());
	}


	return 0;
}
