#include "../include/tiobj.hpp"

using namespace std;


int main(){
	TiObj obj;
	obj["str"] = "String";
	obj["int"] = 100;
	obj["dbl"] = 0.9;
	obj["class"] = "Pkg:Item";

	TiVar& num = obj["int"];
	num = 10 + 20;

	obj->save("teste.ti", "ti");
	obj->save("teste.tb", "binary");
	obj->save("teste.json", "json");
	obj->save("teste.yaml", "yaml");

	return 0;
}
