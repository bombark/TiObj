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
	this->type = TYPE_NULL;
}

TiVar::TiVar(string name){
	this->type = TYPE_NULL;
	this->name = name;
}

TiVar::~TiVar(){
	// Nao colocar para deletar o objeto e vetor aqui, pois essa
	// rotina eh chamada pelo std::vector, quando realoca o vetor
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
}


int  TiVar::getType()  {return this->type;}
bool TiVar::isNull()   {return this->type == TYPE_NULL;}
bool TiVar::isString() {return this->type == TYPE_STRING;}
bool TiVar::isFloat()  {return this->type == TYPE_FLOAT;}
bool TiVar::isInt()    {return this->type == TYPE_INT;}
bool TiVar::isObject() {return this->type == TYPE_OBJECT;}
bool TiVar::isVector() {return this->type == TYPE_VECTOR;}


string TiVar::Str(){
	return this->str;
}

long int TiVar::Int(){
	return this->num;
}

double TiVar::Dbl(){
	return this->dbl;
}

TiObj& TiVar::Obj(){
	return *this->objptr;
}

TiVector& TiVar::Vet(){
	return *this->vetptr;
}



void TiVar::operator=(string value){
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	if ( value[0] == '$' ){
		char* ptr = &value[1];
		TiParser parser;
		TiObj& obj = *( new TiObj() );
		parser.loadText(ptr);
		parser.parse(obj);
		this->objptr = &obj;
		this->type   = TYPE_OBJECT;
	} else {
		this->str = value;
		this->type   = TYPE_STRING;
	}
}

/*void TiVar::operator=(int value){
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	this->num = value;
	this->type   = TYPE_INT;
}*/

void TiVar::operator=(long int value){
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	this->num = value;
	this->type   = TYPE_INT;
}

void TiVar::operator=(double value){
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	this->dbl = value;
	this->type   = TYPE_FLOAT;
}

void TiVar::operator=(TiObj& obj){
	if ( &obj == this->objptr )
		return;
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	this->type   = TYPE_OBJECT;
	this->objptr = &obj;
	obj.count_ref += 1;
}

void TiVar::operator=(TiVector& vector){
	if ( this->type == TYPE_OBJECT )
		this->removeObject();
	this->type   = TYPE_VECTOR;
	this->vetptr = &vector;
}

void TiVar::operator=(TiVar& attr){
	if ( attr.type == TYPE_STRING )
		this->str = attr.str;
	if ( attr.type == TYPE_OBJECT )
		attr.objptr->count_ref += 1;
	this->objptr = attr.objptr;
	this->type   = attr.type;
}

string TiVar::toString(){
	if (this->type == TYPE_STRING){
		string res = "\"" + this->str + "\"";
		return res;
	} else if (this->type == TYPE_INT){
		return std::to_string(this->num); 
	} else if (this->type == TYPE_FLOAT){
		return std::to_string(this->dbl); 
	} else if (this->type == TYPE_OBJECT ){
		return this->objptr->encode(0,true);
	} else if (this->type == TYPE_VECTOR ){
		TiVector* vetor = (TiVector*) this->objptr;
		vetor->encode(0);
	}
}

string TiVar::encode(int tab){

	string res = "";
	if (this->type == TYPE_STRING){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name + " = \"" + this->str + "\";\n";
	} else if (this->type == TYPE_INT){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name; 
		res += " = "; 
		res += std::to_string(this->num); 
		res += ";\n";
	} else if (this->type == TYPE_FLOAT){
		for (int i=1; i<tab; i++)
			 res += '\t';
		res += this->name; 
		res += " = ";
		res += std::to_string(this->dbl); 
		res += ";\n";
	} else if (this->type == TYPE_OBJECT ){
		for (int i=1; i<tab; i++)
			 res += '\t';
		res += this->name+" = ";
		res += this->objptr->encode(tab,false);
	} else if (this->type == TYPE_VECTOR ){
		for (int i=1; i<tab; i++)
			 res += '\t';
		TiVector* ptr = (TiVector*) this->objptr;
		res += this->name+" = ";
		res += ptr->encode(tab,false);
		res += "\n";
	} else if ( this->type == TYPE_TEXT ){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name + " = " +this->strtype+ "<|"+this->str+"|>\n";
	}
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

void TiObj::set(string name, TiVector& value){
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
		if ( in_var.isString() )
			this->classe = in_var.str;
	} else {
		TiVar& var = this->at(in_var.name);
		var = var;
	}
}


void TiObj::setText(string name, string strtype, string text){
	TiVar& var = this->at(name);
	var = text;
	var.type = TYPE_TEXT;
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
	TiVector* novo = new TiVector();
	novo->load(value);
	var = *novo;
}



void TiObj::select(TiBox& out, string classes, string where){
	string token;
	std::vector<std::string> vetclasse;
    std::istringstream iss(classes);
    while ( std::getline(iss, token, ',') ){
        vetclasse.push_back(std::move(token));
    }

	out.clear();
	if ( where == "" ){
		// Executa a busca linear
		for (int i=0; i<this->box.size(); i++){
			TiObj& obj = this->box[i];
			for (int j=0; j<vetclasse.size(); j++){
				if ( obj.is(vetclasse[j]) ){
					out.push_back(&obj);
				}
			}
		}
	} else {
		// Constroi a lista de predicados
		/* DEIXAR COMENTADO ENQUANTO ARRUMA O TILEX
		
		int type;
		TiLex lex;
		TiVar predicate;
		lex.loadText(where);
		lex.next(token, type);
		predicate.name = token;
		lex.next(token, type);
		lex.next(token, type);
		if ( type == TiLex::L_CHAR )
			predicate = token;
		else if ( type == TiLex::L_INT )
			predicate = atoi(token.c_str());
		else if ( type == TiLex::L_FLOAT )
			predicate = atof(token.c_str());
		
		// Executa a busca linear
		for (int i=0; i<this->box.size(); i++){
			TiObj& obj = this->box[i];
			for (int j=0; j<vetclasse.size(); j++){
				if ( obj.is(vetclasse[j]) && obj.has(predicate.name ) ){
					if ( type == TiLex::L_CHAR && predicate.str == obj.atStr(predicate.name) )
						out.push_back(&obj);
					else if ( type == TiLex::L_INT && predicate.num == obj.atInt(predicate.name) )
						out.push_back(&obj);
					else if ( type == TiLex::L_FLOAT && predicate.dbl == obj.atDbl(predicate.name) )
						out.push_back(&obj);
				}
			}
		}
		
		*/
		
		
	}
}


bool box_sort_function (TiObj* a,TiObj* b) { return (a->classe < b->classe);}
void TiObj::sort(){
	std::sort (this->box.begin(), this->box.end(), box_sort_function);
}


bool TiObj::has(std::string name){
	//TiVar& var = this->at(name);
	//return !var.isNull();
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
	if ( var.isInt() )
		return std::to_string(var.num);
	if ( var.isFloat() )
		return std::to_string(var.dbl);
	else
		return var.str;
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
	if ( attr.isString() ){
		return attr.str;
	} else if ( attr.isInt() ){
		return std::to_string(attr.num);
	} else if ( attr.isFloat() ){
		return std::to_string(attr.dbl);
	} else if ( attr.isObject() ){
		return attr.objptr->encode(0);
	} else if ( attr.isVector() ){
		TiVector* tmp = (TiVector*) attr.objptr;
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

int TiVector::load(string text){
	return true;
}

int TiVector::size(){
	return this->itens.size();
}

void TiVector::add(string value){
	TiVar& attr = *( new TiVar("") );
	attr = value;
	this->itens.push_back(&attr);
}

void TiVector::add(long int value){
	TiVar& attr = *( new TiVar("") );
	attr = value;
	this->itens.push_back(&attr);
}

void TiVector::add(double value){
	TiVar& attr = *( new TiVar("") );
	attr = value;
	this->itens.push_back(&attr);
}

void TiVector::add(TiVar& item){
	TiVar& attr = *( new TiVar("") );
	attr = item;
	this->itens.push_back(&attr);
}

void TiVector::addObject(TiObj* object){
	TiVar* attr = new TiVar("");
	attr->objptr = object;
	attr->type   = TYPE_OBJECT;
	this->itens.push_back(attr);
}

void TiVector::addObject(string text){
	TiVar* attr = new TiVar("");
	attr->type   = TYPE_OBJECT;
	attr->objptr = new TiObj(text);
	this->itens.push_back(attr);
}

TiObj* TiVector::find(std::string value){
	for (int i=0; i<this->size(); i++){
		TiVar* item = this->itens[i];
		if ( item->isObject() ){
			TiObj* obj = item->objptr;
			if ( obj->atStr("name") == value ){
				return obj;
			}
		}
	}
	return NULL;
}



string TiVector::encode(int tab, bool indent, bool jmpline){
	if (this == NULL)
		return "[]";

	string res = "";
	if ( indent == true ){ 
		for (int i=1; i<tab; i++)
			res += '\t';
	}
	res += "[";
	for (int i=0; i<this->itens.size(); i++){
		int type = this->itens[i]->type;
		if ( type == TYPE_OBJECT ){
			TiObj* tmp = this->itens[i]->objptr; 
			res += tmp->encode(tab+1, true, false);
		} else if ( type == TYPE_VECTOR ){
			TiVector* vetor = (TiVector*) this->itens[i]->objptr;
			res += vetor->encode(tab+1, true, false);
		} else {
			res += this->itens[i]->toString();
			res += ";";
		}				
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
	if ( var.isFloat() )
		return os << var.dbl;
	if ( var.isString() )
		return os << var.str;
	if ( var.isInt() )
		return os << var.num;
	else if ( var.isVector() )
		return os << var.vetptr->encode();
	else if ( var.isObject() )
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
	/*TiVector vetor = var1.at<TiVector>("imagens");
	for (int i=0; i<vetor.size(); i++){
		cout << vetor.at<string>(i) << endl;
	}*/

	/*TiVector vetor;
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



