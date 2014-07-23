#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include "tiobj.hpp"

using namespace std;


int main(int argc, char **argv){
	if ( argc < 2 ){
		cerr << "Syntax: " << argv[0] << " CampName1,CampName2,... [File]\n";
		return 1;
	}

	TiObj tion;
	string camp = argv[1];
	if ( argc == 2 ){
		tion.loadFile(stdin);
	} else {
		tion.loadFile(argv[2]);
	}

	
	char token[1024];
	int i, cursor;
	if ( camp.size() > 1024 ){
		return 1;
	}
	for (i=0,cursor=0; i<camp.size(); i++){
		char c = camp[i];
		if ( c == ','){
			token[cursor] = '\0';
			if ( strcmp(token, "@") == 0 ){
				cout << tion.encode();
			} else {
				cout << tion.toString(token) << endl;
			}
			cursor = 0;
		} else {
			token[cursor++] = c;
		}
	}
	if ( token > 0 ){
		token[cursor] = '\0';
		if ( strcmp(token, "@") == 0 ){
			cout << tion.encode();
		} else {
			cout << tion.toString(token) << endl;
		}
	}
	return 0;
}
