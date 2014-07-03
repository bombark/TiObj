#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include "tiobj.hpp"

using namespace std;


int main(int argc, char **argv){
	if ( argc < 2 ){
		cerr << "Syntax: " << argv[0] << " ClassName1,ClassName2,...  [from File]\n";
		return 1;
	}

	TiObj obj, aux;
	string field = argv[1];
	if ( argc == 2 ){
		obj.loadFile(stdin);
	} else {
		obj.loadFile(argv[2]);
	}

	obj["name"] = "opa";
	obj["idade"] = 10;
	obj["guer"] = "$opa{name='felipe';}";

	cout << obj;



	//string a = obj.box[0]["name"].Str();
	//cout << a << endl;

	//cout << obj.at("nome") << endl;	


	/*char token[1024];
	int i, cursor;
	if ( field.size() > 1024 ){
		return 1;
	}
	for (i=0,cursor=0; i<field.size(); i++){
		char c = field[i];
		if ( c == ','){
			token[cursor] = '\0';
			if ( strcmp(token, "@") == 0 ){
				cout << obj.encode();
			} else {
				obj.select(aux, token);
				cout << aux << endl;
			}
			cursor = 0;
		} else {
			token[cursor++] = c;
		}
	}
	if ( token > 0 ){
		token[cursor] = '\0';
		if ( strcmp(token, "@") == 0 ){
			cout << obj.encode();
		} else {
			obj.select(aux, token);
			cout << aux << endl;
		}
	}*/
	return 0;
}
