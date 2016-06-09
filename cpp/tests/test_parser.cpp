#include <iostream>
#include "../include/tiobj.hpp"

using namespace std;


int main(int argc, char** argv){
	TiObj b(false, "../tests/big.ti");

	//TiObj a = b.select("Block");
	cout << b.box(0).at("code");

	return 0;
}
