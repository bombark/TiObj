/*  This file is part of Library TiObj.
 *
 *  Copyright (C) 2015  Felipe Gustavo Bombardelli <felipebombardelli@gmail.com>
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
 

/*=====================================================================================*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include "../include/tiasm.hpp"
#include "../include/tiobj.hpp"

using namespace std;

TiVar TiVar::ObjNull(0);

/*
	Nao colocar o TiVar em uma estrutura de vector<TiVar>, pois para cada realocação
	do vetor, executa-se a rotina de delete, entao ocorre varios double free;
/*-------------------------------------------------------------------------------------*/





/*=====================================================================================*/

TiVar::TiVar(){
//cout << "create\n";
	this->type = TiVar::EMPTY;

}

TiVar::TiVar(int isReadOnly){
//cout << "create null\n";
	this->type = TiVar::NULO;
}

TiVar::TiVar(string name){
//cout << "create " << name << endl;
	this->type = TiVar::EMPTY;
	this->name = name;
}

TiVar::~TiVar(){
	if ( this->isObj() ){
		this->removeObject();
	}
}

std::string TiVar::Str(){
	if ( this->isInt() )
		return std::to_string(this->num);
	else if ( this->isDbl() )
		return std::to_string(this->dbl);
	else if ( this->isDbl() )
		return std::to_string(this->dbl);
	else
		return this->str;
}






void TiVar::operator=(string value){
	if ( this->isNull() )
		return;
	if ( this->isObj() )
		this->removeObject();
	this->str  = value;
	this->type = TiVar::STR;
}


void TiVar::operator=(long int value){
	if ( this->isNull() )
		return;
	if ( this->isObj() )
		this->removeObject();
	this->num  = value;
	this->type = TiVar::INT;
}

void TiVar::operator=(double value){
	if ( this->isNull() )
		return;
	if ( this->isObj() )
		this->removeObject();
	this->dbl  = value;
	this->type = TiVar::DBL;
}

void TiVar::operator=(TiObj obj){
	if ( this->isNull() )
		return;
	if ( this->isObj() ){
		if ( obj == this->objptr )
			return;
		this->removeObject();
	}
	this->type   = TiVar::OBJ;
	this->objptr = obj;
}


void TiVar::setBinary(void* ptr, size_t size){
	if ( this->isNull() )
		return;
	if ( this->isObj() ){
		this->removeObject();
	}
	this->type   = TiVar::BINARY;
	
	//printf("%p %d\n", ptr, size);
	
	string buf((const char*)ptr, size);
	
	//printf("%p\n", buf.data());
	this->str = buf;
}





/*void TiVar::operator=(TiVar& attr){
	if ( this->isNull() )
		return;
	if ( this->isObj() )
		this->removeObject();
	if ( attr.isStr() )
		this->str = attr.str;
	if ( attr.isObj() )
		attr.objptr->count_ref += 1;
	this->objptr = attr.objptr;
	this->type   = attr.type;
}*/

std::string TiVar::toString(){
	if ( this->isNull() ){
		return "\"(NULL)\"";
	} else if ( this->isStr() ){
		size_t hasline = this->str.find ('\n');
		if ( hasline == string::npos )
			return "\"" + this->str + "\"";
		else
			return "<|" + this->str + "|>";
	} else if (this->isInt() ){
		return std::to_string(this->num); 
	} else if (this->isDbl() ){
		return std::to_string(this->dbl); 
	} else if (this->isObj() ){
		return this->objptr.encode(0,true);
	} else if (this->isVet() ){

	} else if ( this->isBin() ){
		return "<" + to_string(this->str.size()) + "|" + this->str + "\n";
	}
}





void TiVar::encode(std::string& out, int tab){
	for (int i=1; i<tab; i++)
		out += '\t';
	out += this->name;
	out += " = ";

	if (this->isStr() ){
		out += this->toString();
		out += ";\n";
	} else if (this->isInt() ){
		out += std::to_string(this->num); 
		out += ";\n";
	} else if (this->isDbl()){
		out += std::to_string(this->dbl); 
		out += ";\n";
	} else if ( this->isObj() ){
		this->objptr.encode(out, tab,false);
	} else if (this->isVet() ){
		
	} else if ( this->isBin() ){
		out += this->toString();
	}
}


void TiVar::removeObject(){
	//if ( this->objptr->magic == 0xCAFECAFE ){
		//delete this->objptr;
		//this->objptr = NULL;
	//}
}

void TiVar::toAsm(TiAsm& res){
	if (this->isStr() ){
		res.printStr(this->name, this->str);
	} else if (this->isInt() ){
		res.printInt(this->name, this->num);
	} else if (this->isDbl()){
		res.printDbl(this->name, this->dbl);
	} else if ( this->isObj() ){
		res.printVarObj(this->name, this->objptr.classe() );
		this->objptr.ptr->toAsm(res);
		res.printRet();
	} else if (this->isVet() ){

	} /********* else if ( this->type == TYPE_TEXT ){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name + " = " +this->strtype+ "<|"+this->str+"|>\n";
	}*/
}

/*-------------------------------------------------------------------------------------*/
