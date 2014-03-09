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

/*-------------------------------------------------------------------------------------*/






/*=====================================================================================*/

TiAttr::TiAttr(){
}

TiAttr::TiAttr(string name){
	this->name = name;
}

int TiAttr::getType(){
	return this->type;
}
int TiAttr::isString(){
	return this->type == TYPE_STRING;
}
int TiAttr::isFloat(){
	return this->type == TYPE_FLOAT;
}
int TiAttr::isInt(){
	return this->type == TYPE_INT;
}
int TiAttr::isObject(){
	return this->type == TYPE_OBJECT;
}
int TiAttr::isVector(){
	return this->type == TYPE_VECTOR;
}

int TiAttr::getInt(){
	return this->ivalue;
}

double TiAttr::getFloat(){
	return this->fvalue;
}

string TiAttr::getString(){
	return this->svalue;
}

void TiAttr::set(string value){
	this->svalue = value;
	this->type   = TYPE_STRING;
}

void TiAttr::set(int value){
	this->ivalue = value;
	this->type   = TYPE_INT;
}

void TiAttr::set(double value){
	this->fvalue = value;
	this->type   = TYPE_FLOAT;
}

void TiAttr::set(TiAttr& attr){
	this->svalue = attr.svalue;
	this->ivalue = attr.ivalue;
	this->fvalue = attr.fvalue;
	this->objptr = attr.objptr;
	this->type   = attr.type;
}

void TiAttr::set(TiVector* vector){
	this->type   = TYPE_VECTOR;
	this->objptr = (TiObj*) vector;
}


string TiAttr::toString(){
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

string TiAttr::encode(int tab){
	string res = "";
	if (this->type == TYPE_STRING){
		for (int i=0; i<tab; i++)
			res += '\t';
		res += this->name + " = \"" + this->svalue + "\";\n";
	} else if (this->type == TYPE_INT){
		for (int i=0; i<tab; i++)
			res += '\t';
		res += this->name; 
		res += " = "; 
		res += std::to_string(this->ivalue); 
		res += ";\n";
	} else if (this->type == TYPE_FLOAT){
		for (int i=0; i<tab; i++)
			 res += '\t';
		res += this->name; 
		res += " = ";
		res += std::to_string(this->fvalue); 
		res += ";\n";
	} else if (this->type == TYPE_OBJECT ){
		for (int i=0; i<tab; i++)
			 res += '\t';
		res += this->name+" = ";
		res += this->objptr->encode(tab,false);
	} else if (this->type == TYPE_VECTOR ){
		for (int i=0; i<tab; i++)
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

void TiObj::clear(){
	this->itens.clear();
	this->box.clear();
}

int TiObj::loadFile(FILE* fd){
	this->clear();
}

TiAttr* TiObj::getAttr(string name){
	/*if ( name == "" )
		return NULL;
	if ( name == this->last_name )
		return this->last_ptr;*/

	map<string,TiAttr*>::iterator it;
	it = this->itens.find(name);
	if ( it == this->itens.end() ){
		return NULL;
	} else {
		this->last_name = name;
		this->last_ptr  = it->second;
		return it->second;
	}
}

int TiObj::loadFile(string filename){
	TiParser parser;
	parser.loadFile(filename);
	parser.parse(*this);
}

void TiObj::set(string name, string value){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		attr = new TiAttr(name);
		this->itens.insert( pair<string,TiAttr*>(name, attr) );
	}
	attr->set(value);
}

void TiObj::set(string name, int value){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		attr = new TiAttr(name);
		this->itens.insert( pair<string,TiAttr*>(name, attr) );
	}
	attr->set(value);
}

void TiObj::set(string name, double value){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		attr = new TiAttr(name);
		this->itens.insert( pair<string,TiAttr*>(name, attr) );
	}
	attr->set(value);
}

void TiObj::set(string name, TiVector* value){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		attr = new TiAttr(name);
		this->itens.insert( pair<string,TiAttr*>(name, attr) );
	}
	attr->set(value);
}

void TiObj::set(string name, TiObj* value){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		attr = new TiAttr(name);
		this->itens.insert( pair<string,TiAttr*>(name, attr) );
	}
	attr->type   = TYPE_OBJECT;
	attr->objptr = value;
}

void TiObj::setObject(string name, string text){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		attr = new TiAttr(name);
		this->itens.insert( pair<string,TiAttr*>(name, attr) );
	}
	attr->type   = TYPE_OBJECT;
	TiObj* novo  = new TiObj(text);
	attr->objptr = novo;
}

void TiObj::setVector(string name, string value){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		attr = new TiAttr(name);
		this->itens.insert( pair<string,TiAttr*>(name, attr) );
	}
	attr->type     = TYPE_VECTOR;
	TiVector* novo = new TiVector();
	novo->load(value);
	attr->objptr = (TiObj*) novo;
}

void TiObj::set(TiAttr& newattr){
	TiAttr* attr = this->getAttr(newattr.name);
	if ( attr == NULL ){
		attr = new TiAttr(newattr.name);
		this->itens.insert( pair<string,TiAttr*>(newattr.name, attr) );
	} 
	attr->set(newattr);
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
		TiAttr predicate;
		lex.loadText(where);
		lex.next(token, type);
		predicate.name = token;
		lex.next(token, type);
		lex.next(token, type);
		if ( type == TiLex::L_CHAR )
			predicate.set(token);
		else if ( type == TiLex::L_INT )
			predicate.set(atoi(token.c_str()));
		else if ( type == TiLex::L_FLOAT )
			predicate.set(atof(token.c_str()));
		
		// Executa a busca linear
		for (int i=0; i<this->box.size(); i++){
			TiObj* obj = this->box[i];
			for (int j=0; j<vetclasse.size(); j++){
				if ( obj->is(vetclasse[j]) && obj->has(predicate.name ) ){
					if ( type == TiLex::L_CHAR && predicate.svalue == obj->at<string>(predicate.name) )
						out.addObject(obj);
					else if ( type == TiLex::L_INT && predicate.ivalue == obj->at<int>(predicate.name) )
						out.addObject(obj);
					else if ( type == TiLex::L_FLOAT && predicate.fvalue == obj->at<double>(predicate.name) )
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
	TiAttr* attr = this->getAttr(name);
	return (attr==NULL)?false:true;
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

string TiObj::atStr(string name){
	TiAttr* attr = this->getAttr(name);
	if ( attr == NULL ){
		cerr << "Error{msg='Field " << name << " not found'};\n";
		return "";
	}
	return attr->svalue;
}

/*template<typename _Tp> _Tp& TiObj::at(string index){
	int i = this->find(index);
	assert(i != -1);
	string tptype = typeid(_Tp).name();

	TiAttr* attr = this->itens[i];
	if ( tptype == "i"){
		return (_Tp&) attr->ivalue;
	} else if ( tptype == "Ss" ){
		return (_Tp&) attr->svalue;
	} else if ( attr->type == TYPE_OBJECT || attr->type == TYPE_VECTOR ){
		return (_Tp&) attr->objptr;
	}
}*/

string TiObj::toString(string name){
	map<string,TiAttr*>::iterator it;
	it = this->itens.find(name);

	TiAttr* attr = it->second;
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
		for (int i=0; i<tab; i++)
			res += '\t';
	}
	if ( this->classe != "" )
		res += this->classe + " ";
	res += "{\n";

	for (auto& item: this->itens) {
		res += item.second->encode(tab+1);
	}

	for (int i=0; i<this->box.size(); i++){
		res += this->box[i]->encode(tab+1);
	}

	for (int i=0; i<tab; i++)
		res += '\t';
	res += "}";
	if (jmpline == true)
		res += '\n';
	return res;
}


int TiObj::decode(TiObj& out, string text){
	out.clear();
	return true;
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
	TiAttr* attr = new TiAttr("");
	attr->set(value);
	this->itens.push_back(attr);
}

void TiVector::add(   int value){
	TiAttr* attr = new TiAttr("");
	attr->set(value);
	this->itens.push_back(attr);
}

void TiVector::add(double value){
	TiAttr* attr = new TiAttr("");
	attr->set(value);
	this->itens.push_back(attr);
}

void TiVector::add(TiAttr* item){
	TiAttr* attr = new TiAttr("");
	attr->ivalue = item->ivalue;
	attr->fvalue = item->fvalue;
	attr->svalue = item->svalue;
	attr->objptr = item->objptr;
	attr->type   = item->type;
	this->itens.push_back(attr);
}

void TiVector::addObject(TiObj* object){
	TiAttr* attr = new TiAttr("");
	attr->objptr = object;
	attr->type   = TYPE_OBJECT;
	this->itens.push_back(attr);
}

void TiVector::addObject(string text){
	TiAttr* attr = new TiAttr("");
	attr->type   = TYPE_OBJECT;
	attr->objptr = new TiObj(text);
	this->itens.push_back(attr);
}

TiObj* TiVector::find(std::string value){
	for (int i=0; i<this->size(); i++){
		TiAttr* item = this->itens[i];
		if ( item->isObject() ){
			TiObj* obj = item->objptr;
			if ( obj->at<string>("name") == value ){
				return obj;
			}
		}
	}
	return NULL;
}

/*template<typename _Tp> _Tp& TiVector::at(int pos){
	string tptype = typeid(_Tp).name();
	TiAttr* attr = this->itens[pos];
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
		for (int i=0; i<tab; i++)
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



