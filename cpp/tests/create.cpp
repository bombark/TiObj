#include <iostream>
#include <tiobj.hpp>

using namespace std;


int main(){
	TiObj a;
	for (int i=0; i<10000; i++){
		a.set( Join("teste%s").at(i).ok, "algum teste nao conveniente de tamanho fixo" );
	}
	a.save("var_name_10000.ti");
}
