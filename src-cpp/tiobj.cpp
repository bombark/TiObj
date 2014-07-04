/*=====================================================================================*/

#include "tiobj.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <typeinfo>
#include <sstream>
#include <algorithm>

// Temporario
#include "tiparser.cpp"

TiVar TiVar::OBJNULL; 
TiObj TiObj::ObjNull;

/*-------------------------------------------------------------------------------------*/






/*=====================================================================================*/

TiVar::TiVar(){
	this->type = TYPE_NULL;
}

TiVar::TiVar(string name){
	this->type = TYPE_NULL;
	this->name = name;
}

TiVar::~TiVar(){
	if ( this->type == TYPE_OBJECT )
		delete this->objptr;
	if ( this->type == TYPE_VECTOR )
		delete this->vetptr;
}


int  TiVar::getType()  {return this->type;}
bool TiVar::isNull()   {return this->type == TYPE_NULL;}
bool TiVar::isString() {return this->type == TYPE_STRING;}
bool TiVar::isFloat()  {return this->type == TYPE_FLOAT;}
bool TiVar::isInt()    {return this->type == TYPE_INT;}
bool TiVar::isObject() {return this->type == TYPE_OBJECT;}
bool TiVar::isVector() {return this->type == TYPE_VECTOR;}


string TiVar::Str(){
	return this->svalue;
}

int TiVar::Int(){
	return this->ivalue;
}

double TiVar::Dbl(){
	return this->fvalue;
}

TiObj& TiVar::Obj(){
	return *this->objptr;
}

TiVector& TiVar::Vet(){
	return *this->vetptr;
}



void TiVar::operator=(string value){
	if ( value[0] == '$' ){
		char* ptr = &value[1];
		TiParser parser;
		TiObj& obj = *( new TiObj() );
		parser.loadText(ptr);
		parser.parse(obj);
		this->objptr = &obj;
		this->type   = TYPE_OBJECT;
	} else {
		this->svalue = value;
		this->type   = TYPE_STRING;
	}
}

void TiVar::operator=(int value){
	this->ivalue = value;
	this->type   = TYPE_INT;
}

void TiVar::operator=(double value){
	this->fvalue = value;
	this->type   = TYPE_FLOAT;
}

void TiVar::operator=(TiObj& obj){
	this->type   = TYPE_OBJECT;
	this->objptr = &obj;
}

void TiVar::operator=(TiVector& vector){
	this->type   = TYPE_VECTOR;
	this->vetptr = &vector;
}

void TiVar::operator=(TiVar& attr){
	if ( attr.type == TYPE_STRING )
		this->svalue = attr.svalue;
	this->objptr = attr.objptr;
	this->type   = attr.type;
}

string TiVar::toString(){
	if (this->type == TYPE_STRING){
		string res = "\"" + this->svalue + "\"";
		return res;
	} else if (this->type == TYPE_INT){
		return std::to_string(this->ivalue); 
	} else if (this->type == TYPE_FLOAT){
		return std::to_string(this->fvalue); 
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
		res += this->name + " = \"" + this->svalue + "\";\n";
	} else if (this->type == TYPE_INT){
		for (int i=1; i<tab; i++)
			res += '\t';
		res += this->name; 
		res += " = "; 
		res += std::to_string(this->ivalue); 
		res += ";\n";
	} else if (this->type == TYPE_FLOAT){
		for (int i=1; i<tab; i++)
			 res += '\t';
		res += this->name; 
		res += " = ";
		res += std::to_string(this->fvalue); 
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
	}
	return res;
}

/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/

TiObj::TiObj(){
	this->classe = "";
	this->last_name = "";
}

TiObj::TiObj(string text){
	this->classe = text;
	TiParser parser;
	parser.loadText(text);
	parser.parse(*this);
}

TiObj::~TiObj(){
	for (auto& item: this->attrs) {
		delete item.second;
	}
	for (int i=0; i<this->box.size(); i++)
		delete this->box[i];
}

void TiObj::clear(){
	this->attrs.clear();
	this->box.clear();
}

int TiObj::loadFile(FILE* fd){
	this->clear();
	TiParser parser;
	parser.loadFile(fd);
	parser.parse(*this);
}

int TiObj::loadFile(string filename){
	this->clear();
	TiParser parser;
	parser.loadFile(filename);
	parser.parse(*this);
}

int TiObj::loadStream(FILE* fd){
	this->clear();
	TiParser parser;
	parser.loadFile(fd);
	do {
		TiObj* item = new TiObj();
		if ( parser.parse(*item) ){
			this->addObject(item);
		} else {
			// EM CASO DE ERRO DEVE FAZER ALGO
			break;
		}
	} while(true);
	return true;
}

int TiObj::loadStream(string filename){
	this->clear();
	TiParser parser;
	parser.loadFile(filename);
	do {
		TiObj* item = new TiObj();
		if ( parser.parse(*item) ){
			this->addObject(item);
		} else {
			// EM CASO DE ERRO DEVE FAZER ALGO
			break;
		}
	} while(true);
	return true;
}


TiVar& TiObj::at(string name){
	/*if ( name == "" )
		return NULL;
	if ( name == this->last_name )
		return this->last_ptr;*/

	map<string,TiVar*>::iterator it;
	it = this->attrs.find(name);
	if ( it == this->attrs.end() ){
		TiVar* var = new TiVar(name);
		this->attrs.insert( pair<string,TiVar*>(name,var) );
		return *var;
	} else {
		this->last_name = name;
		this->last_ptr  = it->second;
		return *it->second;
	}
}

void TiObj::set(string name, string value){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		var = *( new TiVar(name) );
		this->attrs.insert( pair<string,TiVar*>(name, &var) );
	}
	var = value;
}

void TiObj::set(string name, int value){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		var = *( new TiVar(name) );
		this->attrs.insert( pair<string,TiVar*>(name, &var) );
	}
	var = value;
}

void TiObj::set(string name, double value){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		var = *( new TiVar(name) );
		this->attrs.insert( pair<string,TiVar*>(name, &var) );
	}
	var = value;
}

void TiObj::set(string name, TiVector& value){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		var = *( new TiVar(name) );
		this->attrs.insert( pair<string,TiVar*>(name, &var) );
	}
	var = value;
}

void TiObj::set(string name, TiObj& value){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		var = *( new TiVar(name) );
		this->attrs.insert( pair<string,TiVar*>(name, &var) );
	}
	var = value;
}

void TiObj::setObject(string name, string text){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		var = *( new TiVar(name) );
		this->attrs.insert( pair<string,TiVar*>(name, &var) );
	}
	var = *( new TiObj(text) );
}

void TiObj::setVector(string name, string value){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		var = *( new TiVar(name) );
		this->attrs.insert( pair<string,TiVar*>(name, &var) );
	}
	TiVector* novo = new TiVector();
	novo->load(value);
	var = *novo;
}

void TiObj::set(TiVar& newattr){
	TiVar& var = this->at(newattr.name);
	if ( var.isNull() ){
		var = *( new TiVar(newattr.name) );
		this->attrs.insert( pair<string,TiVar*>(newattr.name, &var) );
	} 
	var = newattr;
}

void TiObj::addObject(TiObj* obj){
	this->box.push_back(obj);
}


void TiObj::addObject(string text){
	TiObj* obj = new TiObj(text);
	this->box.push_back(obj);
}


void TiObj::select(TiObj& out, string classes, string where){
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
			TiObj* obj = this->box[i];
			for (int j=0; j<vetclasse.size(); j++){
				if ( obj->is(vetclasse[j]) ){
					out.addObject(obj);
				}
			}
		}
	} else {
		// Constroi a lista de predicados
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
			TiObj* obj = this->box[i];
			for (int j=0; j<vetclasse.size(); j++){
				if ( obj->is(vetclasse[j]) && obj->has(predicate.name ) ){
					if ( type == TiLex::L_CHAR && predicate.svalue == obj->atStr(predicate.name) )
						out.addObject(obj);
					else if ( type == TiLex::L_INT && predicate.ivalue == obj->atInt(predicate.name) )
						out.addObject(obj);
					else if ( type == TiLex::L_FLOAT && predicate.fvalue == obj->atDbl(predicate.name) )
						out.addObject(obj);
				}
			}
		}
	}
}


bool box_sort_function (TiObj* a,TiObj* b) { return (a->classe < b->classe);}
void TiObj::sort(){
	std::sort (this->box.begin(), this->box.end(), box_sort_function);
}


int TiObj::has(std::string name){
	TiVar& var = this->at(name);
	return !var.isNull();
}

int TiObj::is(string name){
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

string TiObj::atStr(string name, string _default){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return _default;
	}
	return var.svalue;
}

int TiObj::atInt(string name, int _default){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return _default;
	}
	return var.ivalue;
}

double TiObj::atDbl (string name, double _default){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return _default;
	}
	return var.fvalue;
}

TiObj& TiObj::atObj (string name){
	TiVar& var = this->at(name);
	if ( var.isNull() ){
		return TiObj::ObjNull;
	}
	return *var.objptr;
}


string TiObj::toString(string name){
	map<string,TiVar*>::iterator it;
	it = this->attrs.find(name);

	TiVar* attr = it->second;
	if ( attr->isString() ){
		return attr->svalue;
	} else if ( attr->isInt() ){
		return std::to_string(attr->ivalue);
	} else if ( attr->isFloat() ){
		return std::to_string(attr->fvalue);
	} else if ( attr->isObject() ){
		return attr->objptr->encode(0);
	} else if ( attr->isVector() ){
		TiVector* tmp = (TiVector*) attr->objptr;
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

	for (auto& item: this->attrs) {
		res += item.second->encode(tab+1);
	}

	for (int i=0; i<this->box.size(); i++){
		res += this->box[i]->encode(tab+1);
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
	return *this->box[id];
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

void TiVector::add(   int value){
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

/*template<typename _Tp> _Tp& TiVector::at(int pos){
	string tptype = typeid(_Tp).name();
	TiVar* attr = this->itens[pos];
	if ( tptype == "i"){
		return (_Tp&) attr->ivalue;
	} else if ( tptype == "Ss" ){
		return (_Tp&) attr->svalue;
	} else if ( attr->type == TYPE_OBJECT ){
		return (_Tp&) (attr->objptr);
	} else if ( attr->type == TYPE_VECTOR ){
		return (_Tp&) (attr->objptr);
	}
}*/


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

TiStream::TiStream(){
	tiparser = (TiParser*) new TiParser();
}

void TiStream::open(FILE* fd){
	TiParser* parser = (TiParser*) this->tiparser;
	parser->loadFile(fd);
}

bool TiStream::next(TiObj& obj){
	TiParser* parser = (TiParser*) this->tiparser;
	return parser->parse(obj);
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
		return os << var.fvalue;
	if ( var.isString() )
		return os << var.svalue;
	if ( var.isInt() )
		return os << var.ivalue;
	else if ( var.isVector() )
		return os << var.vetptr->encode();
	else if ( var.isObject() )
		return os << var.objptr->encode();


	return os << "[UNKNOWN]";
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



