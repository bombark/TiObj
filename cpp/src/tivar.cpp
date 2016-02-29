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
#include "../include/tiasm.hpp"
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

std::string TiVar::atStr(){
	if ( this->isInt() )
		return std::to_string(this->num);
	if ( this->isDbl() )
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

/*void TiVar::operator=(int value){
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	this->num = value;
	this->type   = TYPE_INT;
}*/

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

void TiVar::operator=(TiObj& obj){
	if ( this->isNull() )
		return;
	if ( this->isObj() ){
		if ( &obj == this->objptr )
			return;
		this->removeObject();
	}

	this->type   = TiVar::OBJ;
	this->objptr = &obj;
	obj.count_ref += 1;
}

void TiVar::operator=(TiObj* obj){
	if ( this->isNull() )
		return;
	if ( this->isObj() ){
		if ( obj == this->objptr )
			return;
		this->removeObject();
	}
	this->type   = TiVar::OBJ;
	this->objptr = obj;
	obj->count_ref += 1;
}


void TiVar::operator=(TiVet& vector){
	if ( this->isNull() )
		return;
	if ( this->isObj() )
		this->removeObject();
	this->type   = TiVar::VET;
	this->vetptr = &vector;
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
		return this->objptr->encode(0,true);
	} else if (this->isVet() ){
		TiVet* vetor = (TiVet*) this->objptr;
		vetor->encode(0);
	}
}

string TiVar::encode(int tab){

	string res = "";
	if (this->isStr() ){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name + " = " + this->toString() + ";\n";

	} else if (this->isInt() ){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name; 
		res += " = "; 
		res += std::to_string(this->num); 
		res += ";\n";
	} else if (this->isDbl()){
		for (int i=1; i<tab; i++)
			 res += '\t';
		res += this->name; 
		res += " = ";
		res += std::to_string(this->dbl); 
		res += ";\n";
	} else if ( this->isObj() ){
		for (int i=1; i<tab; i++)
			 res += '\t';
		res += this->name+" = ";
		res += this->objptr->encode(tab,false);
	} else if (this->isVet() ){
		for (int i=1; i<tab; i++)
			 res += '\t';
		TiVet* ptr = (TiVet*) this->objptr;
		res += this->name+" = ";
		res += ptr->encode(tab,false);
		res += "\n";
	} /********* else if ( this->type == TYPE_TEXT ){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name + " = " +this->strtype+ "<|"+this->str+"|>\n";
	}*/
	return res;
}


void TiVar::removeObject(){
	if ( this->objptr->count_ref <= 1 ){
		//if ( this->objptr->magic == 0xCAFECAFE ){
			delete this->objptr;
			this->objptr = NULL;
		//}
	} else 
		this->objptr->count_ref -= 1;
}

void TiVar::tiasm(TiTextAsm& res){
	if (this->isStr() ){
		res.printStr(this->name, this->str);
	} else if (this->isInt() ){
		res.printInt(this->name, this->num);
	} else if (this->isDbl()){
		res.printInt(this->name, this->dbl);

	} else if ( this->isObj() ){
			res.printVarObj(this->name, this->objptr->classe);
	
	
/*fazer		if ( this->objptr->classe == "" ){
			res += 'e';
			res += ' ';
			printStr(res,this->name);
		} else {
			res += 'f';
			res += ' ';
			printStr(res,this->name);
			printStr(res,this->objptr->classe);
		}
		this->objptr->tiasm(res);*/
		res.printRet();
	} else if (this->isVet() ){

	} /********* else if ( this->type == TYPE_TEXT ){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name + " = " +this->strtype+ "<|"+this->str+"|>\n";
	}*/
}

/*-------------------------------------------------------------------------------------*/
