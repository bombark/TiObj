#include <iostream>
#include "tiobj.hpp"

using namespace std;


int main(){
	TiObj obj, tmp;
	obj.loadFile("teste.ti");
	obj.select(tmp, "Funcionario", "joao");
	cout << tmp << endl;
}
