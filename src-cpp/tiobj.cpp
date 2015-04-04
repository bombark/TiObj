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

#include "tiobj.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <typeinfo>
#include <sstream>
#include <algorithm>
#include <errno.h>
#include <string.h>

// Temporario
#include "tiparser.cpp"

TiVar TiVar::ObjNull; 
TiObj TiObj::ObjNull;

/*-------------------------------------------------------------------------------------*/


/* OBSERVACAO
 * Existe um mecanismo para liberacao dos objetos
 * Se objeto nao estiver mais sendo refenciado por nenhum objeto, seja em 
 * atributo ou seja no Box, entao ele eh destruido quando for chamado a funcao
 * clear ou delete
 * 
 * Entretanto existe um caso particular, que vai dar erro, se atribuir o 
 * objeto em algum lugar e depois manualmente dar um delete
 * 
 * EVITAR usar o delete manualmente



/*=====================================================================================*/

TiVar::TiVar(){
	this->type = TiVar::EMPTY;
}

TiVar::TiVar(int isReadOnly){
	this->type = TiVar::NULO;
}

TiVar::TiVar(string name){
	this->type = TiVar::EMPTY;
	this->name = name;
}

TiVar::~TiVar(){
	// Nao colocar para deletar o objeto e vetor aqui, pois essa
	// rotina eh chamada pelo std::vector, quando realoca o vetor
	if ( this->isObj() )
		this->removeObject();
}

string TiVar::atStr(){
	if ( this->isInt() )
		return std::to_string(this->num);
	if ( this->isDbl() )
		return std::to_string(this->dbl);
	else
		return this->str;
}


void TiVar::operator=(string value){
	if ( this->isObj() )
		this->removeObject();
	if ( value[0] == '$' ){
		char* ptr = &value[1];
		TiParser parser;
		TiObj& obj = *( new TiObj() );
		parser.loadText(ptr);
		parser.parse(obj);
		this->objptr = &obj;
		this->type   = TiVar::OBJ;
	} else {
		this->str  = value;
		this->type = TiVar::STR;
	}
}

/*void TiVar::operator=(int value){
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	this->num = value;
	this->type   = TYPE_INT;
}*/

void TiVar::operator=(long int value){
	if ( this->isObj() )
		this->removeObject();
	this->num = value;
	this->type   = TiVar::INT;
}

void TiVar::operator=(double value){
	if ( this->isObj() )
		this->removeObject();
	this->dbl  = value;
	this->type = TiVar::DBL;
}

void TiVar::operator=(TiObj& obj){
	if ( &obj == this->objptr )
		return;
	if ( this->isObj() )
		this->removeObject();
	this->type   = TiVar::OBJ;
	this->objptr = &obj;
	obj.count_ref += 1;
}

void TiVar::operator=(TiVet& vector){
	if ( this->isObj() )
		this->removeObject();
	this->type   = TiVar::VET;
	this->vetptr = &vector;
}

void TiVar::operator=(TiVar& attr){
	if ( attr.isStr() )
		this->str = attr.str;
	if ( attr.isObj() )
		attr.objptr->count_ref += 1;
	this->objptr = attr.objptr;
	this->type   = attr.type;
}

string TiVar::toString(){
	if ( this->isStr() ){
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
	if ( this->objptr->count_ref <= 0 ){
		delete this->objptr;
		this->objptr = NULL;
	} else 
		this->objptr->count_ref -= 1;
}

/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/

TiObj::TiObj(){
	this->count_ref = 0;
	this->classe = "";
	this->last_name = "";
	this->varpkg.reserve(16);
}

TiObj::TiObj(string text){
	this->count_ref = 0;
	this->varpkg.reserve(16);
	TiParser parser;
	parser.loadText(text);
}

TiObj::~TiObj(){
	this->classe = "!DEL";
	this->varpkg.clear();
	this->box.clear();
}

void TiObj::clear(){
	this->varpkg.clear();
	this->box.clear();
}


int TiObj::loadText(std::string text){
	this->clear();
	TiParser parser;
	parser.loadText(text);
	parser.parse(*this);
}

int TiObj::loadFile(FILE* fd){
	this->clear();
	TiParser parser;
	parser.loadFile(fd);
	parser.parse(*this);
}

int TiObj::loadFile(string filename){
	FILE* fd = fopen(filename.c_str(), "r");
	if ( !fd ){
		this->clear();
		this->classe="ERROR";
		this->set("msg", strerror(errno));
		this->set("file", filename);
		return 0;
	}
	this->loadFile(fd);
	fclose(fd);
}

int  TiObj::saveFile(string filename){
	string aux;
	aux = this->encode();
	FILE* fd = fopen(filename.c_str(), "w");
	if ( !fd )
		return 1;
	fwrite(aux.c_str(), sizeof(char), aux.size(), fd);
	fclose(fd);
	return 0;
}

int TiObj::loadStream(FILE* fd){
	this->clear();
	TiParser parser;
	parser.loadFile(fd);
	do {
		TiObj* item = new TiObj();
		if ( parser.parse(*item) ){
			this->box += item;
		} else {
			// EM CASO DE ERRO DEVE FAZER ALGO
			break;
		}
	} while(true);
	return true;
}

int TiObj::loadStream(string filename){
	/*this->clear();
	TiParser parser;
	parser.loadFile(filename);
	do {
		TiObj* item = new TiObj();
		if ( parser.parse(*item) ){
			this->box += item;
		} else {
			// EM CASO DE ERRO DEVE FAZER ALGO
			break;
		}
	} while(true);
	return true;*/
}


TiVar& TiObj::at(string name){
	if ( name == "" )				// -- DEVE COLOCAR O OBJNULL como somente leitura!!!
		return TiVar::ObjNull;
	if ( name == this->last_name )
		return varpkg[last_id];

	// Search if already exists the field
	for (int i=0; i<varpkg.size(); i++){
		if ( name == varpkg[i].name ){
			this->last_id   = i;
			this->last_name = name;
			return varpkg[i];
		}
	}

	// Create a new field, case dont exist
	int id = this->varpkg.size();
	this->varpkg.push_back( TiVar(name) );
	this->last_id   = id;
	this->last_name = name;
	return this->varpkg[id];
}

void TiObj::set(string name, string value){

	if ( name == "class" ){
		this->classe = value;
	} else {
		TiVar& var = this->at(name);
		var = value;
	}
}

void TiObj::set(string name, int value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		TiVar& var = this->at(name);
		var = (long int)value;
	}
}

void TiObj::set(string name, long int value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		TiVar& var = this->at(name);
		var = value;
	}
}

void TiObj::set(string name, double value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		TiVar& var = this->at(name);
		var = value;
	}
}

void TiObj::set(string name, TiVet& value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		TiVar& var = this->at(name);
		var = value;
	}
}

void TiObj::set(string name, TiObj& value){
	if ( name == "class" ){
		this->classe = "";
	} else {
		TiVar& var = this->at(name);
		var = value;
	}
}

void TiObj::set(TiVar& in_var){
	if ( in_var.name == "class" ){
		this->classe = "";
		if ( in_var.isStr() )
			this->classe = in_var.str;
	} else {
		TiVar& var = this->at(in_var.name);
		var = var;
	}
}


void TiObj::setText(string name, string strtype, string text){
	TiVar& var = this->at(name);
	var = text;

	//var.type = TYPE_TEXT;
	var.type = TiVar::STR;

	if ( strtype.size() >= sizeof(TiVar::strtype) ){
		strncpy(var.strtype, strtype.c_str(), sizeof(TiVar::strtype)-1 );
		var.strtype[ sizeof(TiVar::strtype) ] = '\0';
	} else
		strcpy(var.strtype, strtype.c_str());
}

void TiObj::setObject(string name, string text){
	/*TiVar& var = this->at(name);
	if ( var.isNull() ){
		int id = this->varpkg.size();
		this->varpkg.push_back( TiVar(name) );
		var = this->varpkg[id];
	}
	var = ;*/
}

void TiObj::setVector(string name, string value){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		int id = this->varpkg.size();
		this->varpkg.push_back( TiVar(name) );
		var = this->varpkg[id];
	}
	TiVet* novo = new TiVet();
	novo->load(value);
	var = *novo;
}



TiObj& TiObj:: select(TiObj& out, std::string classes){
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









bool TiObj::has(std::string name){
	if ( name=="class" )
		return true;
	for (int i=0; i<varpkg.size(); i++){
		if ( name == varpkg[i].name ){
			this->last_id   = i;
			this->last_name = name;
			return true;
		}
	}
	return false;
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
	if ( var.isNull() ){
		return _default;
	}
	return var.num;
}

double TiObj::atDbl (string name, double _default){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return _default;
	}
	return var.dbl;
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


TiVar& TiObj::operator[](string name){
	return this->at(name);
}

TiObj& TiObj::operator[](int id){
	return this->box[id];
}

/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/

int TiVet::load(string text){
	return true;
}

int TiVet::size(){
	return this->itens.size();
}

void TiVet::add(string value){
	TiVar& attr = *( new TiVar("") );
	attr = value;
	this->itens.push_back(&attr);
}

void TiVet::add(long int value){
	TiVar& attr = *( new TiVar("") );
	attr = value;
	this->itens.push_back(&attr);
}

void TiVet::add(double value){
	TiVar& attr = *( new TiVar("") );
	attr = value;
	this->itens.push_back(&attr);
}

void TiVet::add(TiVar& item){
	TiVar& attr = *( new TiVar("") );
	attr = item;
	this->itens.push_back(&attr);
}

void TiVet::addObject(TiObj* object){
	TiVar* attr  = new TiVar("");
	attr->objptr = object;
	attr->type   = TiVar::OBJ;
	this->itens.push_back(attr);
}

void TiVet::addObject(string text){
	TiVar* attr  = new TiVar("");
	attr->type   = TiVar::OBJ;
	attr->objptr = new TiObj(text);
	this->itens.push_back(attr);
}

TiObj* TiVet::find(std::string value){
	for (int i=0; i<this->size(); i++){
		TiVar* item = this->itens[i];
		if ( item->isObj() ){
			TiObj* obj = item->objptr;
			if ( obj->atStr("name") == value ){
				return obj;
			}
		}
	}
	return NULL;
}



string TiVet::encode(int tab, bool indent, bool jmpline){
	if (this == NULL)
		return "[]";

	string res = "";
	if ( indent == true ){ 
		for (int i=1; i<tab; i++)
			res += '\t';
	}
	res += "[";
	for (int i=0; i<this->itens.size(); i++){
		/*int type = this->itens[i]->type;
		if ( type == TYPE_OBJECT ){
			TiObj* tmp = this->itens[i]->objptr; 
			res += tmp->encode(tab+1, true, false);
		} else if ( type == TYPE_VECTOR ){
			TiVet* vetor = (TiVet*) this->itens[i]->objptr;
			res += vetor->encode(tab+1, true, false);
		} else {
			res += this->itens[i]->toString();
			res += ";";
		}*/
	}
	res += "]";
	//if (jmpline == true)
	//	res += '\n';
	return res;
}

/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/


void   TiBox::clear(){
	for (int i=0; i<this->size(); i++){
		TiObj* obj = this->at(i);
		if ( obj->count_ref <= 0 )
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
	return os << obj.encode();
}

ostream& operator<<(ostream& os, TiVar& var){

	if ( var.isNull() )
		return os << "[NULL]";
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

//int main(){
	/*TiObj var1("Pessoa");
	var1.set("idade", 20);
	var1.set("nome", "Felipe Gustavo Bombardelli");
	var1.set("idade", 40);
	var1.set("lugar", "Curitiba");
	var1.at<string>("nome") = "felipe";
	var1.setObject("pai","{name='clovis bombardelli'; nasc='10/12/2012'}");
	var1.setVector("imagens","['felipe';'fernando';'joao']");
	cout << var1.encode();
	*/
	/*TiVet vetor = var1.at<TiVet>("imagens");
	for (int i=0; i<vetor.size(); i++){
		cout << vetor.at<string>(i) << endl;
	}*/

	/*TiVet vetor;
	vetor.addObject("{name='felipe'}");
	vetor.addObject("{name='fernando'}");
	vetor.addObject("{name='diogo'}");
	cout << vetor.encode();
	TiObj var2;
	var2.set("idade", 20);
	var2.set("nome", "Felipe Gustavo Bombardelli");
	var2.set("idade", 40);
	var2.set("lugar", "Curitiba");
	var2.set("files", vetor);	
	cout << var2.encode();*/
	
	/*TiObj TiObj("Variable {coma='int a'};");
	TiObj::decode(TiObj, "File:Package {name='felipe';idade=22.45;pai=Pessoa{name='clovis bombardelli'; pai=Pessoa{name='alfredo'}}}");
	if ( TiObj.is("Package") )
	cout << TiObj.encode();*/

	/*TiObj TiObj;
	TiObj.load("teste.TiObj");
	cout << TiObj.encode();*/
//}



