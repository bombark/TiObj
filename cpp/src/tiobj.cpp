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

#include "../include/tiobj.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <typeinfo>
#include <sstream>
#include <algorithm>
#include <errno.h>
#include <string.h>
#include "../include/tiasm.hpp"

using namespace std;


TiObj TiObj::ObjNull;

/*-------------------------------------------------------------------------------------*/



/*=====================================================================================*/

TiObj::TiObj(){
	this->magic = 0xCAFECAFE;
	this->count_ref = 0;
	this->classe = "";
}

TiObj::TiObj(const char* text){
	this->magic = 0xCAFECAFE;
	this->count_ref = 0;
	this->loadText(text);
}

TiObj::TiObj(string text){
	this->magic = 0xCAFECAFE;
	this->count_ref = 0;
	this->loadText(text);
}

TiObj::~TiObj(){
	this->magic = 0;
	this->clear();
}

void TiObj::clear(){
	this->varpkg.clear();
	this->box.clear();
}


int TiObj::loadText(const char* text){
	this->clear();
	if ( text == NULL )
		return 0;
	string buffer;
	parseText(buffer, text);
	parseTiAsm(*this, buffer);
	return 1;
}

int TiObj::loadText(std::string text){
	string buffer;
	this->clear();
	parseText(buffer, text);
	parseTiAsm(*this, buffer);
	return 1;
}

int TiObj::loadFile(FILE* fd){
	string buffer;
	this->clear();
	parseFileFd(buffer, fd);
	parseTiAsm(*this, buffer);
	return 1;
}

int TiObj::loadFile(string filename){
	FILE* fd = fopen(filename.c_str(), "r");
	if ( !fd ){
		this->clear();
		this->classe="Error";
		this->set("msg", strerror(errno));
		this->set("file", filename);
		return 0;
	}
	this->loadFile(fd);
	fclose(fd);
	return 1;
}

int  TiObj::saveFile(string filename){
	string aux;
	aux = this->encode();
	FILE* fd = fopen(filename.c_str(), "w");
	if ( !fd )
		return 0;
	fwrite(aux.c_str(), sizeof(char), aux.size(), fd);
	fclose(fd);
	return 1;
}


TiVar& TiObj::createVar(std::string name){
	return this->varpkg.push( 0, name );
}


TiVar& TiObj::at(string name, bool create){
	if ( name == "" )
		return TiVar::ObjNull;
	TiVar& var = this->varpkg[name];
	if ( var.isNull() && create ){
		return this->createVar(name);
	}
	return var;
}


void TiObj::set(string name, string value){
	if ( name == "class" ){
		this->classe = value;
	} else {
		this->varpkg.push( value, name );
	}
}

void TiObj::set(string name, int value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		this->varpkg.push( value, name );
	}
}

void TiObj::set(string name, long int value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		this->varpkg.push( value, name );
	}
}

void TiObj::set(string name, double value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		this->varpkg.push( value, name );
	}
}

void TiObj::set(string name, TiVet& value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		//this->varpkg.push( value, name );
	}
}

void TiObj::set(string name, TiObj& value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		this->varpkg.push( value, name );
	}
}

void TiObj::set(std::string name, TiObj* value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		this->varpkg.push( value, name );
	}
}

void TiObj::set(TiVar& in_var){
	/*if ( in_var.name == "class" ){
		this->classe = "";
		if ( in_var.isStr() )
			this->classe = in_var.str;
	} else {
		TiVar* var = &this->at(in_var.name);
		if ( var->isNull() )
			var = &this->createVar(in_var.name);

		var->type = in_var.type;
		var->name = in_var.name;
	}*/
}


void TiObj::setText(string name, string strtype, string text){
	TiVar* var = &this->at(name);
	if ( var->isNull() )
		var = &this->createVar(name);

	*var = text;

	//var.type = TYPE_TEXT;
	var->type = TiVar::STR;

	if ( strtype.size() >= sizeof(TiVar::strtype) ){
		strncpy(var->strtype, strtype.c_str(), sizeof(TiVar::strtype)-1 );
		var->strtype[ sizeof(TiVar::strtype) ] = '\0';
	} else
		strcpy(var->strtype, strtype.c_str());
}

void TiObj::setObj(std::string name, std::string text){
	TiObj* novo = new TiObj();
	novo->loadText(text);
	this->varpkg.push( novo, name );
}

void TiObj::setVector(string name, string value){
	/*TiVar& var = this->at(name);
	if ( var.isNull() ){
		int id = this->varpkg.size();
		this->varpkg.push_back( TiVar(name) );
		var = this->varpkg[id];
	}
	TiVet* novo = new TiVet();
	novo->load(value);
	var = *novo;*/
}



TiObj& TiObj::select(TiObj& out, std::string classes){
	string token;
	std::vector<std::string> vetclasse;
	std::istringstream iss(classes);
	while ( std::getline(iss, token, ',') ){
		vetclasse.push_back(std::move(token));
	}

	if ( this != &out ){
		out.clear();
		for (int i=0; i<this->box.size(); i++){
			TiObj& obj = this->box[i];
			for (int j=0; j<vetclasse.size(); j++){
				if ( obj.is(vetclasse[j]) ){
					out.box += &obj;
				}
			}
		}
	} else {
		for (int i=0; i<this->box.size(); ){
			bool erased = false;
			TiObj& obj = this->box[i];
			for (int j=0; j<vetclasse.size(); j++){
				if ( !obj.is(vetclasse[j]) ){
					out.box.erase( out.box.begin()+i );
					erased = true;
					break;
				}
			}
			if ( !erased )
				i += 1;
		}
	}

	return out;
}



int partition(TiBox& box, uint left, uint right, std::string& field){
	TiObj*  aux;
	TiObj** boxdata = &box.at(0);
	if ( !box[left].has(field) ){
		aux = boxdata[right-1];
		boxdata[right-1] = boxdata[left];
		boxdata[left] = aux;
		return right-1;
	}

	uint i = left;
	uint j;
	string j_value;
	string value = box[left].atStr(field);
	for(j=left+1; j<right; j++){
		if ( !box[j].has(field) )
			continue;

		j_value = box[j].atStr(field);
		if( strcasecmp(j_value.c_str(), value.c_str()) < 0  ){
			i=i+1;
			aux = boxdata[i];
			boxdata[i] = boxdata[j];
			boxdata[j] = aux;
		}
	}
	aux = boxdata[i];
	boxdata[i] = boxdata[left];
	boxdata[left] = aux;

	return i;
}


void quickSort(TiBox& box, uint left, uint right, std::string& field){
	uint pivot;
	if(left<right){
		pivot = partition(box,left,right,field);
		quickSort(box,left,pivot,field);
		quickSort(box,pivot+1,right,field);
	}
}

TiObj& TiObj::orderby(TiObj& out, std::string field){
	for (int i=0; i<this->box.size(); i++){
		out.box += this->box[i];
	}
	quickSort (out.box, 0, this->box.size(), field);
	return out;
}

TiObj& TiObj::orderby(std::string field){
	quickSort (this->box, 0, this->box.size(), field);
	return *this;
}


void TiObj::groupby(TiObj& out, std::string field){
	out.clear();
	for (int i=0; i<this->box.size(); i++){
		TiObj& node = this->box[i];
		if ( !node.has(field) ){
			out.box += node;
			continue;
		}

		TiObj* cur;
		string key = node.atStr(field);
		if ( out.has(key) ){
			cur = &out.atObj(key);
		} else {
			cur = new TiObj();
			out.set(key, cur);
		}
		cur->box += node;
	}
}






bool TiObj::has(std::string name){
	if ( name=="class" )
		return true;
	return this->varpkg.has(name);
}

bool TiObj::is(string name){
	char token[1024];
	int cursor, i;
	for (i=0, cursor=0; i<this->classe.size(); i++){
		char c = this->classe[i];
		if (c == ':'){
			token[cursor] = '\0';
			if (name == token){
				return true;
			}
			cursor = 0;
		} else {
			token[cursor++] = c;
		}
	}
	token[cursor] = '\0';
	if ( name == token )
		return true;
	return false;
}

bool TiObj::isOnly(string name){
	if ( name == this->classe )
		return true;
	return false;
}


string TiObj::atStr(string name, string _default){
	if ( name == "class" )
		return this->classe;
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return _default;
	}

	return var.atStr();
}

long int TiObj::atInt(string name, long int _default){
	TiVar& var = this->at(name);
	if ( var.isNull() )
		return _default;
	if ( var.isDbl() )
		return var.dbl;
	if ( var.isInt() )
		return var.num;
	if ( var.isStr() )
		return atoi(var.str.c_str());
	return _default;
}

double TiObj::atDbl (string name, double _default){
	TiVar& var = this->at(name);
	if ( var.isNull() )
		return _default;
	if ( var.isDbl() )
		return var.dbl;
	if ( var.isInt() )
		return var.num;
	if ( var.isStr() )
		return atof(var.str.c_str());
	return _default;
}

TiObj& TiObj::atObj (string name){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return TiObj::ObjNull;
	}
	return *var.objptr;
}


string TiObj::toString(){
	string res;
	res += this->classe+"{";
	for (int i=0; i<this->varpkg.size(); i++){
		res += varpkg[i].name+"="+varpkg[i].toString()+";";
	}


	/*

		FALTA MOSTRAR OS OBJETOS DO BOX --------------------------------------------------------------------------------


	 */


	res += "}";
	return res;
}


string TiObj::toString(string name){
	TiVar& attr = this->at(name);
	if ( attr.isStr() ){
		return attr.str;
	} else if ( attr.isInt() ){
		return std::to_string(attr.num);
	} else if ( attr.isDbl() ){
		return std::to_string(attr.dbl);
	} else if ( attr.isObj() ){
		return attr.objptr->encode(0);
	} else if ( attr.isVet() ){
		TiVet* tmp = (TiVet*) attr.objptr;
		return tmp->encode(0);
	}
	return "";
}

string TiObj::encode(int tab, bool indent, bool jmpline){
	string res = "";
	if ( indent == true ){ 
		for (int i=1; i<tab; i++)
			res += '\t';
	}

	if ( tab == 0 ){
		if ( this->classe != "" )
			res += "class = \""+this->classe+"\";\n";
	} else {
		if ( this->classe != "" )
			res += this->classe + " ";
		res += "{\n";
	}
	
	for (int i=0; i<this->varpkg.size(); i++) {
		res += varpkg[i].encode(tab+1);
	}

	for (int i=0; i<this->box.size(); i++){
		res += this->box[i].encode(tab+1);
	}

	if ( tab == 0 ){
	} else {
		for (int i=1; i<tab; i++)
			res += '\t';
		res += "}";
	}
	if (jmpline == true)
		res += '\n';
	return res;
}


int TiObj::decode(TiObj& out, string text){
	out.clear();
	return true;
}

void TiObj::tiasm(TiTextAsm& res){
	for (uint i=0; i<this->length(); i++){
		TiVar& var = (*this)[i];
		var.tiasm(res);
	}
	for (uint i=0; i<this->size(); i++){
		TiObj& obj = this->box[i];
		res.printObj(obj.classe);
		obj.tiasm(res);
		res.printRet();
	}
}


std::string TiObj::tiasm(){
	TiTextAsm res;
	this->tiasm(res);
	return res.text;
}

/*-------------------------------------------------------------------------------------*/





/*=====================================================================================*/


void   TiBox::clear(){
	for (int i=0; i<this->size(); i++){
		TiObj* obj = this->at(i);
		if ( obj->count_ref <= 1 )
			delete obj;
		else
			obj->count_ref -= 1;
	}
}

TiObj& TiBox::next(){
}

TiObj& TiBox::operator[](int id){
	return *this->at(id);
}

void TiBox::operator+=(TiObj& obj){
	this->push_back(&obj);
	obj.count_ref += 1;
}

void TiBox::operator+=(TiObj* obj){
	this->push_back(obj);
	obj->count_ref += 1;
}

void TiBox::operator+=(string objstr){
	this->push_back( new TiObj(objstr) );
}

/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/

ostream& operator<<(ostream& os, TiObj& obj){
	return os << obj.encode(0,true,false);
}

ostream& operator<<(ostream& os, TiVar& var){
	if ( var.isNull() )
		return os << "[NULL]";
	if ( var.isEmpty() )
		return os << "[EMPTY]";
	if ( var.isDbl() )
		return os << var.dbl;
	if ( var.isStr() )
		return os << var.str;
	if ( var.isInt() )
		return os << var.num;
	else if ( var.isVet() )
		return os << var.vetptr->encode();
	else if ( var.isObj() )
		return os << var.objptr->encode();
	return os << "[UNKNOWN]";
}

ostream& operator<<(ostream& os, TiBox& box){
	for (int i=0; i<box.size(); i++){
		os << box[i].encode(1);
	}
}

/*-------------------------------------------------------------------------------------*/





TiStream::TiStream(FILE* fd){
	parser.loadFile(fd);
}


bool TiStream::next(TiObj& out){
	parser.parseStream();
}














