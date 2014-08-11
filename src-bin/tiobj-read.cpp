#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <tiarg.hpp>
#include "tiobj.hpp"


using namespace std;


int main(int argc, char **argv){
	/*TiObj obj("name='felipe';Pessoa{campo='aaa'}Pessoa{name='felipe';tmp=10teste=20}");
	cout << obj;
	return 0;*/
	
	TiObj args, obj;
	getArgs(args, argc, argv);
	string field = args.atStr("_Akk");
	string from  = args.atStr("from");
	
	
	if ( from == "" ){
		obj.loadFile(stdin);
	} else {
		obj.loadFile(from);
	}
	
	char token[1024];
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
				cout << obj.toString(token) << endl;
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
			cout << obj.toString(token) << endl;
		}
	}
	return 0;
}
