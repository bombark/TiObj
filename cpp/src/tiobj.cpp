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
#include <sys/file.h>
#include <unistd.h>
#include "../include/tiparser.hpp"

using namespace std;


_TiObj _TiObj::ObjNull;
//TiPool<_TiObj> _TiObj::pool;

void tibuilder (TiBuffer& buffer, _TiObj* obj);
size_t partition(TiObjPkg& box, size_t left, size_t right, std::string& field);


/*-------------------------------------------------------------------------------------*/



/*======================================  TiObj  ======================================*/

TiObj::TiObj(){
	_TiObj* novo = new _TiObj;
	this->reset(novo);
}

TiObj::TiObj(bool is_lock, std::string filename){
	_TiObj* novo = new _TiObj;
	novo->load(filename, is_lock);
	this->reset(novo);
}

TiObj::TiObj(std::string text){
	_TiObj* novo = new _TiObj(text);
	this->reset(novo);
}

TiObj::TiObj(_TiObj* a){
	this->reset(a);
}

TiObj::TiObj(int a){
	//this->ptr = nullptr;
}

/*-------------------------------------------------------------------------------------*/



/*=====================================  _TiObj  ======================================*/


//******* Constructors and Destructors *******
_TiObj::_TiObj(){}

_TiObj::_TiObj(const char* text){
	this->loadText(text);
}

_TiObj::_TiObj(string text){
	this->loadText(text);
}

_TiObj::~_TiObj(){
	this->clear();
}

void _TiObj::clear(){
	this->classe.clear();
	this->var.clear();
	this->box.clear();
}


//******* Loaders *******
void _TiObj::loadText(const char* text){
	this->clear();
	if ( text == NULL )
		return ;
	TiBufferText buffer(text);
	tibuilder( buffer, this );
}

void _TiObj::loadText(std::string text){
	this->clear();
	TiBufferText buffer(text);
	tibuilder( buffer, this );
}

void _TiObj::load(FILE* fd, bool is_lock){
	this->clear();
	TiBufferFile buffer(fd);
	tibuilder( buffer, this );
}

void _TiObj::load(string filename, bool is_lock){
	FILE* fd = fopen(filename.c_str(), "r");
	if ( !fd ){
		this->clear();
		this->classe="Error";
		this->set("msg", strerror(errno));
		this->set("file", filename);
	}
	this->load(fd,is_lock);
	fclose(fd);
}

void  _TiObj::save(string filename, string format, bool is_lock){
	string aux;
	if ( format == "ti" ){
		this->encode(aux, 0, true, false);
	} else if ( format == "binary" ){
		aux = this->toAsm();
	} else if ( format == "yaml" ){
		aux = this->toYaml();
	} else if ( format == "json" ){
		aux = this->toJson();
	}

	if ( is_lock ){
		FILE* fd = fopen(filename.c_str(), "a");
		assert(fd);
		flock( fileno(fd), LOCK_EX );
		ftruncate(fileno(fd), 0);
		fwrite(aux.c_str(), sizeof(char), aux.size(), fd);
		flock( fileno(fd), LOCK_UN);
		fclose(fd);
	} else {
		FILE* fd = fopen(filename.c_str(), "w");
		assert(fd);
		fwrite(aux.c_str(), sizeof(char), aux.size(), fd);
		fclose(fd);
	}
}




TiVar& _TiObj::at(string name, bool create){
	if ( name == "" )
		return TiVar::ObjNull;
	TiVar& var = this->var[name];
	if ( var.isNull() && create ){
		return this->var.push( "", name );
	}
	return var;
}






void _TiObj::set(TiVar& in_var){
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


void _TiObj::setText(string name, string strtype, string text){
	TiVar* var = &this->at(name);
	//if ( var->isNull() )
	//	var = &this->createVar(name);

	*var = text;

	//var.type = TYPE_TEXT;
	var->type  = TiVar::STR;
	var->strtype = strtype;
}

void _TiObj::setObj(std::string name, std::string text){
	_TiObj* novo = new _TiObj;
	novo->loadText(text);
	this->var.push( novo, name );
}




TiObj _TiObj::select(std::string classes){
	TiObj out;
	string token;
	std::vector<std::string> vetclasse;
	std::istringstream iss(classes);
	while ( std::getline(iss, token, '|') ){
		vetclasse.push_back(std::move(token));
	}
	out->clear();
	for (size_t i=0; i<this->size(); i++){
		TiObj obj = this->box[i];
		for (size_t j=0; j<vetclasse.size(); j++){
			if ( obj->is(vetclasse[j]) ){
				out->box += obj;
			}
		}
	}
	return out;
}

void quickSort(TiObjPkg& box, size_t left, size_t right, std::string& field){
	uint pivot;
	if(left<right){
		pivot = partition(box,left,right,field);
		quickSort(box,left,pivot,field);
		quickSort(box,pivot+1,right,field);
	}
}

TiObj _TiObj::orderby(TiObj out, std::string field){
	for (int i=0; i<this->box.size(); i++){
		out->box += this->box[i];
	}
	quickSort (out->box, 0, this->box.size(), field);
	return out;
}

TiObj _TiObj::orderby(std::string field){
	quickSort (this->box, 0, this->box.size(), field);
	return TiObj(this);
}


TiObj _TiObj::groupby(std::string field){
	TiObj out;
	for (int i=0; i<this->size(); i++){
		TiObj node = this->box[i];
		if ( node->has(field) ){
			TiObj cur(0);
			string key = node->atStr(field);
			if ( out->has(key) ){
				cur = out->atObj(key);
			} else {
				cur.create();
				out->set(key, cur);
			}
			cur->box += node;
		} else {
			out->box += node;
		}
	}
}


bool _TiObj::is(string name){
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

bool _TiObj::isOnly(string name){
	if ( name == this->classe )
		return true;
	return false;
}


string _TiObj::atStr(string name, string _default){
	if ( name == "class" )
		return this->classe;

	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return _default;
	}

	return var.Str();
}

long int _TiObj::atInt(string name, long int _default){
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

double _TiObj::atDbl (string name, double _default){
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

TiObj _TiObj::atObj (string name){
	TiVar& var = this->at(name);
	/*if ( var.isNull() ){
		return TiObj(TiObj::ObjNull);
	}*/
	return var.objptr;
}


string _TiObj::toString(){
	string res;
	res += this->classe;
	res += '{';
	for (int i=0; i<this->var.size(); i++){
		res += var[i].name;
		res += '=';
		res += var[i].toString();
		res += ';';
	}
	res += '}';
	return res;
}


string _TiObj::toString(string name){
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
		//TiVet* tmp = (TiVet*) attr.objptr;
		//return tmp->encode(0);
	}
	return "";
}

void _TiObj::encode(std::string& res, int tab, bool indent, bool jmpline){


	if ( indent == true ){
		for (int i=1; i<tab; i++)
			res += '\t';
	}
	if ( tab == 0 ){
		if ( this->classe != "" ){
			res += "class = \"";
			res += this->classe;
			res += "\";\n";
		}
	} else {
		if ( this->classe != "" ){
			res += this->classe;
			res += ' ';
		}
		res += "{\n";
	}

	for (int i=0; i<this->var.size(); i++) {
		var[i].encode(res, tab+1);
	}

	for (int i=0; i<this->box.size(); i++){
		this->box[i]->encode(res, tab+1);
	}

	if ( tab == 0 ){
	} else {
		for (int i=1; i<tab; i++)
			res += '\t';
		res += '}';
	}
	if (jmpline == true)
		res += '\n';
}


/*int _TiObj::decode(TiObj out, string text){
	//out.clear();
	return true;
}*/

void _TiObj::toAsm(TiAsm& res){
	for (size_t i=0; i<this->length(); i++){
		this->at(i).toAsm(res);
	}
	for (size_t i=0; i<this->size(); i++){
		TiObj obj = this->box[i];
		res.printObj(obj->classe);
		obj->toAsm(res);
		res.printRet();
	}
}

void _TiObj::toJson(std::string& out){
	out += "{\n";

	out += "\"class\":\"";
	out += this->classe;
	out += "\"";

	for (size_t i=0; i<this->length(); i++){
		out += ",\n";
		this->at(i).toJson(out);
	}

	if ( this->size() > 0 ){
		out += ",\n\"box\" : [";
		this->box[0]->toJson(out);
		for (size_t i=1; i<this->size(); i++){
			TiObj obj = this->box[i];
			out += ",\n";
			obj->toJson(out);
		}
		out += "\n]\n";
	}
	out += "}";
}


void _TiObj::toYaml(std::string& out){

	/*out += " - class: ";
	out += this->classe;
	out += "\n";*/

	for (size_t i=0; i<this->length(); i++){
		//out += "   ";
		this->at(i).toYaml(out);
		out += '\n';
	}

	if ( this->size() > 0 ){
		out += "   box:";
		this->box[0]->toJson(out);
		for (size_t i=1; i<this->size(); i++){
			TiObj obj = this->box[i];
			out += "\n";
			obj->toYaml(out);
		}
	}

}

/*-------------------------------------------------------------------------------------*/





/*=====================================================================================*/


/*void   TiObjPkg::clear(){
	for (size_t i=0; i<this->size(); i++){
		delete this->at(i);
	}
}*/

void TiObjPkg::operator+=(string objstr){
	this->push_back( TiObj(new _TiObj(objstr)) );
}

/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/

ostream& operator<<(ostream& os, TiObj obj){
	return os << obj->encode(0,true,false);
}

ostream& operator<<(ostream& os, _TiObj& obj){
	string buf;
	obj.encode(buf, 0,true,false);
	return os << buf;
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
		return os << "[VECTOR]";
	else if ( var.isObj() )
		return os << var.objptr->encode(0);
	return os << "[UNKNOWN]";
}

ostream& operator<<(ostream& os, TiObjPkg& box){
	for (int i=0; i<box.size(); i++){
		os << box[i]->encode(1);
	}
}

/*-------------------------------------------------------------------------------------*/





/*TiStream::TiStream(FILE* fd){
	//this->parser.output->min = 1;
	//this->parser.loadFile(fd);
}*/


//bool TiStream::next(TiObj& out){
	/*if ( this->parser.good() ){

		parser.parseStream();

		_TiObj* ptr = out.ptr.get();
		if ( ptr == nullptr ){
			ptr = new _TiObj();
			out.ptr.reset(ptr);
		} else
			ptr->clear();
		build_tiasm(*ptr, parser.output->text);


		if ( ptr->isEmpty() )
			return false;
		return true;
	}
	return false;*/
//}



size_t partition(TiObjPkg& box, size_t left, size_t right, std::string& field){
	TiObj  aux;
	if ( !box[left]->has(field) ){
		aux = box[right-1];
		box[right-1] = box[left];
		box[left]    = aux;
		return right-1;
	}
	size_t  j, i = left;
	string j_value;
	string value = box[left]->atStr(field);
	for(j=left+1; j<right; j++){
		if ( !box[j]->has(field) )
			continue;
		j_value = box[j]->atStr(field);
		if( strcasecmp(j_value.c_str(), value.c_str()) < 0  ){
			i=i+1;
			aux = box[i];
			box[i] = box[j];
			box[j] = aux;
		}
	}
	aux = box[i];
	box[i] = box[left];
	box[left] = aux;
	return i;
}
