#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;



class Teste {
  public:
	int a,b;
};

class TesteVirtual {
  public:
	int a,b;

	virtual void sum(int a, int b){}
};



int main(int argc, char** argv){
	//----------------------------

	cout << "Teste 1\n";
	cout << "  sem: " << sizeof(Teste) << endl;
	cout << "  com: " << sizeof(TesteVirtual) << endl;


	//----------------------------

	cout << "Teste 2: new Teste\n";
	char block[1024];
	memset(block,0x41,1024);

	Teste* a1 = new (block) Teste;
	cout << "   sem(): " << a1->a << endl;

	Teste* a2 = new (block) Teste();
	cout << "   com(): " << a2->a << endl;

	//----------------------------

	string a = "teste 3";
	string b = a;
	a.clear();
	a.reserve(32);

	//----------------------------

	return 0;
}
