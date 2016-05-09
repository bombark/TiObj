#include "../include/tiobj.hpp"






/*=====================================================================================*/

TiAsm::TiAsm(){
	this->text.reserve(1024);
}

void TiAsm::putInt(long int number){
	char* num = (char*) &number;
	for (uint i=0; i<sizeof(number); i++){
		text.push_back(num[i]);
	}
}

void TiAsm::putDbl(double number){
	char* num = (char*) &number;
	for (uint i=0; i<sizeof(number); i++){
		text.push_back(num[i]);
	}
}

void TiAsm::putStr(std::string in){
	uint  _size = in.size()+1;
	char*  size = (char*) &_size;
	for (uint i=0; i<4; i++){
		this->text.push_back(size[i]);
	}
	this->text += in;
	this->text.push_back('\0');
	uint toalign = _size&0x1;
	for (uint i=0; i<toalign; i++){
		this->text.push_back('\0');
	}
}


void TiAsm::putVarName(std::string name){
	short  _size = name.size()+1;
	char*   size = (char*) &_size;
	for (uint i=0; i<2; i++){
		this->text.push_back(size[i]);
	}
	this->text += name;
	this->text.push_back('\0');
	uint toalign = _size&0x1;
	for (uint i=0; i<toalign; i++){
		this->text.push_back('\0');
	}
}


// Print Complex
void TiAsm::printInt(std::string name, long int num){
	this->putCmd('a');
	this->putVarName(name);
	this->putInt(num);
}

void TiAsm::printDbl(std::string name, double num){
	this->putCmd('b');
	this->putVarName(name);
	this->putDbl(num);
}

void TiAsm::printStr(std::string name, std::string text){
	this->putCmd('c');
	this->putVarName(name);
	this->putStr(text);
}

void TiAsm::printVarObj(std::string name, std::string obj_class){
	if ( obj_class == "" ){
		this->putCmd('e');
		this->putVarName(name);
	} else {
		this->putCmd('f');
		this->putVarName(name);
		this->putVarName(obj_class);
	}
}

void TiAsm::printObj(std::string obj_class){
	if ( obj_class == "" ){
		this->putCmd('g');
	} else {
		this->putCmd('h');
		this->putVarName(obj_class);
	}
}



/*-------------------------------------------------------------------------------------*/

