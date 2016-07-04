#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;


int main(int argc, char** argv){

		TiObj text("name=felipe;idade=28;score=123.123 2222");
		cout << text << endl;

		TiObj file1;
		file1->load( argv[1] );
		cout << file1 << endl;



	//TiObj file2(false,"teste.ti");
	//cout << file2 << endl;

	return 0;
}
