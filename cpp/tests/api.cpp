#include <iostream>
#include "../include/tiparser.hpp"
#include "../include/tiobj.hpp"

using namespace std;


void teste(string a){
	string res;
	parseFile(res, a);
	cout << res;
}


void teste2(){
	TiObj obj;
	obj.create();
	
	obj["name"] = "Felipe";
	obj.set("idade", 10);
	obj.set("ano", 1.222);
	
	
	TiObj aux;
	aux.create();
	aux["name"] = "Clovis";
	aux["idade"] = 61;
	obj["pai"] = aux;
	
	obj.box() += aux;
	cout << obj;
	
	//TiVet* var = &obj.var();
}


int main(int argc, char** argv){
	
	//string a = argv[1];
	
	TiObj obj;
	TiStream stream(stdin);
	while ( stream.next(obj) )
		cout << obj << endl;
	
	
	
	/*TiParser parser;
	
	parser.loadFile(stdin);
	parser.output->min = 1;
	
	_TiObj* a = new _TiObj();
	while ( parser.isGood() ){
		parser.parseStream();
		cout << parser.output->text << endl;
		
		a->clear();
		build_tiasm(*a, parser.output->text);
		
		string aux;
		a->encode(aux,0,false,false);
		cout << aux;
	}*/

	//cout << obj.length() << endl;
	//cout << obj.size() << endl;
	//obj.tiasm();
	
	//cout << obj;
	
	//teste(a);
	//teste(a);
	//teste(a);
	//teste(a);

	return 0;
}
