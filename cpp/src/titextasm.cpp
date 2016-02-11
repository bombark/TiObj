#include "../include/tiobj.hpp"






/*=====================================================================================*/

TiTextAsm::TiTextAsm(){
	this->text.reserve(4096);
	this->text = "TiV1";
}

void TiTextAsm::putInt(long int number){
	char* num = (char*) &number;
	for (uint i=0; i<sizeof(number); i++){
		text.push_back(num[i]);
	}
}

void TiTextAsm::putDbl(double number){
	char* num = (char*) &number;
	for (uint i=0; i<sizeof(number); i++){
		text.push_back(num[i]);
	}
}

void TiTextAsm::putStr(std::string in){
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


void TiTextAsm::putVarName(std::string name){
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
void TiTextAsm::printInt(std::string name, long int num){
	this->putCmd('a');
	this->putVarName(name);
	this->putInt(num);
}

void TiTextAsm::printDbl(std::string name, double num){
	this->putCmd('b');
	this->putVarName(name);
	this->putDbl(num);
}

void TiTextAsm::printStr(std::string name, std::string text){
	this->putCmd('c');
	this->putVarName(name);
	this->putStr(text);
}

void TiTextAsm::printVarObj(std::string name, std::string obj_class){
	if ( obj_class == "" ){
		this->putCmd('e');
		this->putVarName(name);
	} else {
		this->putCmd('f');
		this->putVarName(name);
		this->putVarName(obj_class);
	}
}

void TiTextAsm::printObj(std::string obj_class){
	if ( obj_class == "" ){
		this->putCmd('g');
	} else {
		this->putCmd('h');
		this->putVarName(obj_class);
	}
}



/*-------------------------------------------------------------------------------------*/

