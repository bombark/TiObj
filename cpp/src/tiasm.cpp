/*  This file is part of Library TiObj.
 *
 *  Copyright (C) 2016  Felipe Gustavo Bombardelli <felipebombardelli@gmail.com>
 *
 *  TiObj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



/*=====================================- HEADER -======================================*/

#include "../include/tiobj.hpp"

/*-------------------------------------------------------------------------------------*/



/*=====================================  PUBLIC  ======================================*/

TiAsm::TiAsm(int min){
	this->min = min;
	this->now = 0;
	this->text.reserve(1024);
}


// Print Complex
void TiAsm::printInt(std::string name, long int num){
	if ( this->now < this->min )
		return;
	this->putCmd('a');
	this->putVarName(name);
	this->putInt(num);
}

void TiAsm::printDbl(std::string name, double num){
	if ( this->now < this->min )
		return;
	this->putCmd('b');
	this->putVarName(name);
	this->putDbl(num);
}

void TiAsm::printStr(std::string name, std::string text){
	if ( this->now < this->min )
		return;
	this->putCmd('c');
	this->putVarName(name);
	this->putStr(text);
}

void TiAsm::printVarObj(std::string name, std::string obj_class){
	if ( this->now < this->min )
		return;
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
	this->now += 1;
	if ( obj_class == "" ){
		if ( this->now <= this->min )
			return;
		this->putCmd('g');
	} else {
		if ( this->now <= this->min ){
			this->printStr("class", obj_class);
		} else {
			this->putCmd('h');
			this->putVarName(obj_class);
		}
	}
}

void TiAsm::printRet(){
	this->now -= 1;
	if ( this->now > this->min )
		this->putCmd('i');
}

/*-------------------------------------------------------------------------------------*/




/*====================================  PRIVATE  ======================================*/

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

/*-------------------------------------------------------------------------------------*/

