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

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <exception>

#include "tiasm.hpp"
//#include "tipool.hpp"

class TiObjPkg;
class TiVarPkg;
class TiVar;
class TiBox;
class _TiObj;

/*-------------------------------------------------------------------------------------*/



/*====================================- Exception -====================================*/

class tiexception : public std::exception {
	std::string msg;
  public:
	tiexception(std::string msg) : exception(){
		this->msg = msg;
	}
	virtual const char* what() const throw(){
		return this->msg.c_str();
	}
};

/*-------------------------------------------------------------------------------------*/



/*======================================- TIOBJ -======================================*/

class TiObj : public std::shared_ptr<_TiObj> {

  public:
	TiObj();
	TiObj(int a);
	TiObj(bool is_lock, std::string filename);
	TiObj(std::string text);
	TiObj(_TiObj* a);


	void create();
	//inline void create(std::string text="");
	TiVar& operator[](std::string name);
	TiVar& operator[](size_t id);

};

/*-------------------------------------------------------------------------------------*/


/*====================================- TiObjPkg -=====================================*/

class TiObjPkg : public std::vector<TiObj> {
  public:
	void clear();
	inline TiObj  operator[](size_t id){return this->at(id);}
	inline void   operator+=(TiObj obj){this->push_back(obj);}
	inline void   operator+=(std::string objstr);
};

/*-------------------------------------------------------------------------------------*/



/*======================================- TiVar -======================================*/

class TiVar {
  public:
	enum Type { NULO, EMPTY, STR, INT, DBL, OBJ, VET, BINARY };
	Type type;

	std::string name;
	std::string str;
	std::string strtype;

	union {
		double     dbl;
		long int   num;
	};
	TiObj      objptr;
	std::vector<char> bin;


	TiVar();
	TiVar(int isReadOnly);
	TiVar(std::string name);
	~TiVar();

	inline bool isNull() {return this->type == TiVar::NULO;}
	inline bool isEmpty(){return this->type == TiVar::EMPTY;}
	inline bool isStr()  {return this->type == TiVar::STR;}
	inline bool isDbl()  {return this->type == TiVar::DBL;}
	inline bool isInt()  {return this->type == TiVar::INT;}
	inline bool isObj()  {return this->type == TiVar::OBJ;}
	inline bool isVet()  {return this->type == TiVar::VET;}
	inline bool isBin()  {return this->type == TiVar::BINARY;}


	std::string      Str();
	inline long int  Int(){return this->num;}
	inline double    Dbl(){return this->dbl;}
	inline TiObj     Obj(){return this->objptr;}


	void operator=(std::string value);
	inline void operator=(int value){*this = (long int) value;}
	void operator=(long int value);
	void operator=(double value);
	void operator=(TiObj obj);
	void operator=(std::vector<char>& bin);
	//void operator=(TiVar& attr);

	void setBinary(void* ptr, size_t size);


	std::string toString();

	void encode(std::string& out, int tab=0);
	inline std::string encode(int tab=0){
		std::string out;
		this->encode(out, tab);
		return out;
	}

	void toAsm(TiAsm& res);
	void toJson(std::string& out);
	void toYaml(std::string& out);

	static TiVar ObjNull;



	/*void* operator new(size_t stAllocateBlock){
		return TiVar::pool.malloc();
	}
	void operator delete(void* obj){
		TiVar::pool.free(obj);
	}
	static TiPool<TiVar> pool;*/



  private:
	inline void removeObject();
};

/*-------------------------------------------------------------------------------------*/







/*====================================- TiVarPkg 2 -===================================*/

class TiVarPkg : public std::vector< TiVar* > {
  public:
	TiVarPkg();

	void clear();

	TiVar& push(std::string value, std::string name=""){
		return this->_push<std::string>(value,name);
	}

	TiVar& push(int value, std::string name=""){
		return this->_push<long int>(value,name);
	}

	TiVar& push(long int value, std::string name=""){
		return this->_push<long int>(value,name);
	}

	TiVar& push(double value, std::string name=""){
		return this->_push<double>(value,name);
	}

	TiVar& push(float value, std::string name=""){
		return this->_push<double>(value,name);
	}

	TiVar& push(TiObj value, std::string name=""){
		return this->_push<TiObj>(value,name);
	}

	TiVar& push(std::vector<char>& value, std::string name=""){
		return this->_push< std::vector<char> >(value,name);
	}

	TiVar& push(void* ptr, size_t size, std::string name="");


	TiVar& search(std::string query);

	inline TiVar& operator[](size_t i){return *this->at(i);}
	inline TiVar& operator[](std::string name){return this->search(name);}

	inline bool has(std::string name){
		return !this->search(name).isNull();
	}

	inline TiVar& last(){return *this->at( this->size()-1 );}

  private:
	bool   search(std::string query, TiVar** out_obj, size_t& out_addpos);


	template< typename T >
	inline TiVar& _push(T value, std::string name){
		TiVar* aux;
		size_t order_pos = this->size();
		bool exists = search(name, &aux, order_pos);
		if ( exists ){
			*aux = value;
			return *aux;
		} else {
			TiVar* var = new TiVar();
			*var = value;
			var->name = name;
			this->insert ( this->begin()+order_pos, var );
			return *var;
		}
	}


};

/*---------------------------------------------------------------------------------------*/



/*======================================- TiObj -======================================*/

class _TiObj {
  public:
	std::string classe;
	TiVarPkg var;
	TiObjPkg box;

	_TiObj();
	_TiObj(const char* text);
	_TiObj(std::string text);
	~_TiObj();

	void clear();

	void  loadText(const char* text);
	void  loadText(std::string text);

	void  load(FILE*  fd, bool is_lock=false);
	void  load(std::string filename, bool is_lock=false);
	void  save(std::string filename, std::string format, bool is_lock=false);


	inline void set(std::string name, std::string value){
		this->_set<std::string>(name, value);
	}
	inline void set(std::string name, int value){
		this->_set<long int>(name, value);
	}
	inline void set(std::string name, long int value){
		this->_set<long int>(name, value);
	}
	inline void set(std::string name, size_t value){
		this->_set<long int>(name, value);
	}
	inline void set(std::string name, float value){
		this->_set<double>(name, value);
	}
	inline void set(std::string name, double value){
		this->_set<double>(name, value);
	}
	inline void set(std::string name, TiObj value){
		this->var.push( value, name );
	}
	inline void set(std::string name, std::vector<char>& value){
		this->var.push( value, name );
	}
	inline void setBinary(std::string name, void* ptr, size_t size){
		this->var.push( ptr, size, name );
	}


	/*void set(std::string name, TiVet& value){
		this->_set<std::string>(name, value);
	}*/



	void setObj(std::string name, std::string text);
	void setVector(std::string name, std::string text);
	void set(TiVar& attr);
	void setText(std::string name, std::string strtype, std::string text);



	inline TiVar&      at(size_t i){return this->var[i];}
	       TiVar&      at(std::string name, bool         create=false);

	std::string atStr (std::string name, std::string  _default="");
	long int    atInt (std::string name, long int     _default=0);
	double      atDbl (std::string name, double       _default=0.0);
	TiObj       atObj (std::string name);

	//inline TiVar& operator[](std::string name){return this->at(name,true);}
	//inline TiVar& operator[](int i){return this->varpkg[i];}





	inline bool has(std::string name){
		return (name=="class")? true: this->var.has(name);
	}

	//FAZER: unset(std::string name);


	inline bool isEmpty(){return this->var.size()==0 && this->box.size()==0;}
	inline unsigned length(){return this->var.size();}
	inline unsigned size(){return this->box.size();}

	bool is(std::string name);
	bool isOnly(std::string name);



	TiObj select (std::string classes);

	//TiObj orderby(TiObj out, std::string field);
	void orderby(std::string field);

	//_TiObj& where(_TiObj& out, std::string condpkg);
	//_TiObj& where(std::string condpkg);

	TiObj groupby(std::string field);




	void encode(std::string& out, int tab, bool indent=true, bool jmpline=true);
	inline std::string encode(int tab, bool indent=true, bool jmpline=true){
		std::string out; this->encode(out,tab,indent,jmpline); return out;
	}


	std::string toString();
	std::string toString(std::string name);


	void toAsm(TiAsm& res);
	inline std::string toAsm(){
		TiAsm res;
		res.start();
		this->toAsm(res);
		return res.text;
	}

	void toJson(std::string& out);
	std::string toJson(){
		std::string buf;
		this->toJson(buf);
		return buf;
	}

	void toYaml(std::string& out);
	std::string toYaml(){
		std::string buf = "%YAML:1.0\n";
		this->toYaml(buf);
		return buf;
	}


	static _TiObj ObjNull;


	//size_t id;
	/*friend TiPoolNode<_TiObj>;
	void* operator new(size_t stAllocateBlock){
		return _TiObj::pool.malloc();
	}
	void operator delete(void* _obj){
		_TiObj *obj = static_cast<_TiObj*>(_obj);
		_TiObj::pool.free(obj->id);
	}*/
	//static TiPool<_TiObj> pool;


  private:
	template<typename T>
	void _set(std::string name, T value){
		if ( name == "class" ){
			this->classe = value;
		} else {
			this->var.push( value, name );
		}
	}
};



/*-------------------------------------------------------------------------------------*/






/*==================================- TIOBJ - MACROS -=================================*/

inline void TiObj::create(){
	_TiObj* novo = new _TiObj();
	this->reset(novo);
}

inline TiVar& TiObj::operator[](std::string name){
	return this->get()->at(name,true);
}

inline TiVar& TiObj::operator[](size_t id){
	return this->get()->var[id];
}

inline void TiObjPkg::operator+=(std::string objstr){
	this->push_back( TiObj(new _TiObj(objstr)) );
}

/*-------------------------------------------------------------------------------------*/



/*====================================- TiStream -=====================================*/

#include "tiparser.hpp"

bool tibuilder_step (TiParser& parser, _TiObj* obj);

class TiStream {
	TiBufferFile buffer;
	TiParser     parser;

  public:
	TiStream(FILE* fd){
		this->buffer.load(fd);
		this->parser.load(&buffer);
	}

	inline bool next(TiObj out){
		out->clear();
		return tibuilder_step(parser,out.get());
	}
};

/*-------------------------------------------------------------------------------------*/



/*======================================- Join -=======================================*/

class Join {
  public:
	std::string* format;
	std::string  ok;
	unsigned int pi, size;

	Join(std::string format);

	Join& at(std::string var);
	Join& at(int var);
	Join& at(double var);
	Join& replace(char patt, std::string dst, std::string base);
	Join& trim(std::string var);
};

/*-------------------------------------------------------------------------------------*/



/*====================================- Friends -======================================*/

std::ostream& operator<<(std::ostream& os, TiObj  obj);
std::ostream& operator<<(std::ostream& os, _TiObj& obj);
std::ostream& operator<<(std::ostream& os, TiVar& var);
std::ostream& operator<<(std::ostream& os, TiBox& box);

/*-------------------------------------------------------------------------------------*/
