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
#include <typeinfo>
#include <memory>
#include <map>
#include "tiasm.hpp"

class _TiObj;
class TiObjPkg;
class TiVarPkg;
class TiVar;
class TiBox;

/*-------------------------------------------------------------------------------------*/



/*======================================- TIOBJ -======================================*/

class TiObj {
  public:
	std::shared_ptr<_TiObj> ptr;

  public:
	TiObj();
	TiObj(int a);
	TiObj(bool is_lock, std::string filename);
	TiObj(std::string text);
	TiObj(_TiObj* a);


	inline void  loadText(const char* text);
	inline void  loadText(std::string text);


	inline void  load( FILE* fd,         bool is_lock=false );
	inline void  load( std::string file, bool is_lock=false );
	inline void  save( std::string file, bool is_lock=false );

	void create();
	//inline void create(std::string text="");

	inline std::string& classe();


	inline TiVar& operator[](std::string name);
	inline TiVar& operator[](size_t i);
	inline bool   operator==(TiObj akk);


	inline void        encode(std::string& out, int tab=0, bool indent=true, bool jmpline=true);
	inline std::string encode(int tab=0, bool indent=true, bool jmpline=true);

	inline TiObjPkg& box();
	inline TiObj     box(size_t i);
	inline TiVarPkg& var();
	inline TiVar&    var(size_t i);

	
	inline void set(std::string name, std::string value);
	inline void set(std::string name, int value);
	inline void set(std::string name, long int value);
	inline void set(std::string name, float value);
	inline void set(std::string name, double value);
	inline void set(std::string name, TiObj value);
	inline void setBinary(std::string name, void* ptr, size_t size);

	inline size_t length();
	inline size_t size();
	inline void   clear();

	inline bool isNull();


	inline std::string atStr (std::string name, std::string  _default="");
	inline long int    atInt (std::string name, long int     _default=0);
	inline double      atDbl (std::string name, double       _default=0.0);
	inline TiObj       atObj (std::string name);


	inline bool is  (std::string name);
	inline bool has (std::string name);

	inline std::string toAsm();
	//inline std::string toJson();
	//inline std::string toXml();
};

/*-------------------------------------------------------------------------------------*/







/*======================================- TiVar -======================================*/

class TiVar {
  public:
	enum Type { NULO, EMPTY, STR, INT, DBL, OBJ, VET, BINARY };
	Type type;

	char strtype[32];
	std::string   name;
	std::string   str;
	
	union {
		double     dbl;
		long int   num;
		TiObj      objptr;
	};
	

	
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
	inline long int  Int();
	inline double    Dbl();
	inline TiObj     Obj();


	void operator=(std::string value);
	inline void operator=(int value){*this = (long int) value;}
	void operator=(long int value);
	void operator=(double value);
	void operator=(TiObj obj);
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

	static TiVar ObjNull;


  private:
	inline void removeObject();
};

/*-------------------------------------------------------------------------------------*/



/*====================================- TiObjPkg -=====================================*/

class TiObjPkg : public std::vector<TiObj> {
  public:
	inline TiObj  operator[](size_t id){return this->at(id);}
	inline void   operator+=(TiObj obj){this->push_back(obj);}
	       void   operator+=(std::string objstr);
};

/*-------------------------------------------------------------------------------------*/


/*====================================- TiVarPkg 1 -===================================--

class TiVarPkg {
	std::vector< TiVar* > data;
	std::map< std::string, TiVar* > index;


  public:
	TiVarPkg();
	~TiVarPkg();

	size_t size(){return this->data.size();}

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
	
	TiVar& push(void* ptr, size_t size, std::string name="");


	inline TiVar& operator[](size_t i){return *this->data[i];}
	TiVar& operator[](std::string name);

	inline bool has(std::string name){}   //size_t pos;TiVar* obj;return this->search(name,&obj,&pos);}



	//void        encode(std::string& res, int tab=0, bool indent=true, bool jmpline=true);
	std::string encode(int tab=0, bool indent=true, bool jmpline=true);

  private:
	template< typename T >
	inline TiVar& _push(T value, std::string name){
		TiVar* res;
		auto it = this->index.find ( name );
		if (it != this->index.end()){
			res = it->second;
		} else {
			res = new TiVar();
			res->name = name;
			this->data.push_back( res );
			this->index.insert( std::pair<std::string,TiVar*>(name,res) );
		}
		*res = value;
		return *res;
	}

};

---------------------------------------------------------------------------------------*/




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
	
	TiVar& push(void* ptr, size_t size, std::string name="");


	TiVar& search(std::string query);

	inline TiVar& operator[](size_t i){return *this->at(i);}
	inline TiVar& operator[](std::string name){return this->search(name);}

	inline bool has(std::string name){
		TiVar& obj = this->search(name);
		return obj.isNull();
	}

	inline TiVar& last(){return *this->at( this->size()-1 );}

  private:
	bool   search(std::string query, TiVar** out_obj, size_t& out_addpos);
	

	template< typename T >
	inline TiVar& _push(T value, std::string name){
		TiVar* aux;
		size_t order_pos;
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

	int  loadText(const char* text);
	int  loadText(std::string text);

	int  loadFile(FILE*  fd);
	int  loadFile(std::string filename);
	int  saveFile(std::string filename);


	inline void set(std::string name, std::string value){
		this->_set<std::string>(name, value);
	}
	inline void set(std::string name, int value){
		this->_set<long int>(name, value);
	}
	inline void set(std::string name, long int value){
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




	std::string toString();
	std::string toString(std::string name);




	       bool   has(std::string name);
	inline bool isset(std::string name){return this->has(name);}
	inline bool hasnt(std::string name){return !this->has(name);}
	//FAZER: unset(std::string name);


	inline bool isEmpty(){return this->var.size()==0 && this->box.size()==0;}
	inline unsigned length(){return this->var.size();}
	inline unsigned size(){return this->box.size();}

	bool is(std::string name);
	bool isOnly(std::string name);



	TiObj select (std::string classes);

	TiObj orderby(TiObj out, std::string field);
	TiObj orderby(std::string field);

	//_TiObj& where(_TiObj& out, std::string condpkg);
	//_TiObj& where(std::string condpkg);

	TiObj groupby(std::string field);


	
	
	void encode(std::string& out, int tab, bool indent, bool jmpline);

	static int  decode(TiObj out, std::string text);



	void toAsm(TiAsm& res);
	inline std::string toAsm(){
		TiAsm res;
		res.start();
		this->toAsm(res);
		return res.text;
	}




	static _TiObj ObjNull; 




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
	_TiObj* ptr = new _TiObj();
	this->ptr.reset(ptr);
}

inline void TiObj::loadText(const char* text){
	_TiObj* obj = new _TiObj(text);
	this->ptr.reset(obj);
}

inline void TiObj::loadText(std::string text){
	_TiObj* obj = new _TiObj(text);
	this->ptr.reset(obj);
}

inline void TiObj::load(FILE* fd, bool is_lock){
	_TiObj* obj = new _TiObj();
	obj->loadFile(fd);
	this->ptr.reset(obj);
}

inline void TiObj::load(std::string file, bool is_lock){
	_TiObj* obj = new _TiObj();
	obj->loadFile(file);
	this->ptr.reset(obj);
}

inline void TiObj::save(std::string file, bool is_lock){
	this->ptr->saveFile(file);
}

inline std::string& TiObj::classe(){
	return this->ptr->classe;
}


inline TiVar& TiObj::operator[](size_t i){
	return this->ptr->at(i);
}

inline TiVar& TiObj::operator[](std::string name){
	return this->ptr->at(name, true);
}

inline bool   TiObj::operator==(TiObj akk){
	return this->ptr == akk.ptr;
}

inline void TiObj::encode(std::string& out, int tab, bool indent, bool jmpline){
	this->ptr->encode(out, tab, indent, jmpline);
}

inline std::string TiObj::encode(int tab, bool indent, bool jmpline){
	std::string res;
	this->ptr->encode(res, tab, indent, jmpline);
	return res;
}

inline TiObjPkg& TiObj::box(){
	return this->ptr->box;
}

inline TiObj  TiObj::box(size_t i){
	return this->ptr->box[i];
}

inline TiVarPkg& TiObj::var(){
	return this->ptr->var;
}

inline TiVar& TiObj::var(size_t i){
	return this->ptr->var[i];
}


inline void TiObj::set(std::string name, std::string value){
	this->ptr->set(name, value);
}

inline void TiObj::set(std::string name, int value){
	this->ptr->set(name, value);
}

inline void TiObj::set(std::string name, long int value){
	this->ptr->set(name, value);
}

inline void TiObj::set(std::string name, float value){
	this->ptr->set(name, value);
}

inline void TiObj::set(std::string name, double value){
	this->ptr->set(name, value);
}

inline void TiObj::set(std::string name, TiObj value){
	this->ptr->set(name, value);
}

inline void TiObj::setBinary(std::string name, void* ptr, size_t size){
	this->ptr->setBinary(name, ptr, size);
}

inline size_t TiObj::length(){
	return this->ptr->length();
}

inline size_t TiObj::size(){
	return this->ptr->size();
}

inline void TiObj::clear(){
	this->ptr->clear();
}

inline bool TiObj::isNull(){
	return this->ptr.get() == nullptr;
}

inline std::string TiObj::atStr (std::string name, std::string  _default){
	return this->ptr->atStr(name, _default);
}

inline long int    TiObj::atInt (std::string name, long int     _default){
	return this->ptr->atInt(name, _default);
}

inline double     TiObj::atDbl (std::string name, double       _default){
	return this->ptr->atDbl(name, _default);
}

inline TiObj TiObj::atObj (std::string name){this->ptr->atObj(name);}



inline bool TiObj::is  (std::string name){return this->ptr->is(name);}
inline bool TiObj::has (std::string name){return this->ptr->has(name);}


inline std::string TiObj::toAsm(){return this->ptr->toAsm();}

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
std::ostream& operator<<(std::ostream& os, TiVar& var);
std::ostream& operator<<(std::ostream& os, TiBox& box);

bool build_tiasm(_TiObj& out, std::string data);
bool build_tiasm_str(_TiObj& out, const char* data, size_t total);

/*-------------------------------------------------------------------------------------*/





/*====================================- TiStream -=====================================*/


#include "tiparser.hpp"


class TiStream {
	TiParser parser;

  public:
	TiStream(FILE* fd);
	bool next(TiObj& out);
};



/*-------------------------------------------------------------------------------------*/


