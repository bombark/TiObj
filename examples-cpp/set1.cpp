#include <iostream>
#include "../src-cpp/tiobj.hpp"

using namespace std;


int main(){
	TiObj obj;
	obj.set("name","felipe");
	obj.set("idade",10);
	obj.set("salario",500.5);
	obj["sobrenome"] = "Bombardelli";
	obj["idade"] = 25;

	cout << obj;
	return 0;
}
