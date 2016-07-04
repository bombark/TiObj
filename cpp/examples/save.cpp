#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;



int main(){
	TiObj obj;
	obj->set("id",1);
	obj["jogo"] = "Canastra";
	for (int i=0; i<10; i++){
		obj->box += Join("class=Player;id=%s;score=%s").at(i).at(rand()).ok;
	}
	obj->save("score.ti","ti");
	return 0;
}
