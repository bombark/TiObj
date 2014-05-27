#ifndef TIOBJ_HPP
#define TIOBJ_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

using namespace std;

class TiObj;
class TiVector;

#define TYPE_NULL   0
#define TYPE_STRING 1
#define TYPE_INT    2
#define TYPE_FLOAT  3
#define TYPE_OBJECT 4
#define TYPE_VECTOR 5

class TiAttr {
  public:
	int       type;
	string    name;
	string    svalue;
	union {
		double    fvalue;
		long int  ivalue;
		TiObj*    objptr;
	};

	TiAttr();
	TiAttr(string name);

	int getType();
	bool isNull();
	bool isString();
	bool isFloat();
	bool isInt();
	bool isObject();
	bool isVector();	

	int    getInt();
	double getFloat();
	string getString();

	void set(string value);
	void set(int value);
	void set(double value);
	void set(TiAttr& attr);
	void set(TiVector* vector);

	string toString();
	string encode(int tab);
};

class TiObj {
  private:

	TiAttr* last_ptr;
	string  last_name;

  public:
	string classe;
	map<string,TiAttr*> attrs;
	vector<TiObj*> box;

	TiObj();
	TiObj(string text);
	void clear();
	int  loadFile(FILE*  fd);
	int  loadFile(string filename);
	int  loadStream(FILE* fd);
	int  loadStream(string filename);

	TiAttr* getAttr(string name);
	void set(string name, string value);
	void set(string name, int value);
	void set(string name, double value);
	void set(string name, TiVector* value);
	void set(string name, TiObj* value);
	void setObject(string name, string text);
	void setVector(string name, string text);
	void set(TiAttr& attr);
	
	void addObject(TiObj* obj);
	void addObject(string text);
	void select(TiObj& out, string classe, string where="");
	void sort();

	template<typename _Tp> _Tp& at(string name){
		if ( name == "class" ){
			return (_Tp&) this->classe;
		}

		map<string,TiAttr*>::iterator it;
		it = this->attrs.find(name);
		string tptype = typeid(_Tp).name();
		if ( it == this->attrs.end() ){
			cout << "Error{msg='Field " << name << " not found'};\n";
		}

		TiAttr* attr = it->second;
		if ( tptype == "i"){
			return (_Tp&) attr->ivalue;	
		} else if ( tptype == "d" ){
			return (_Tp&) attr->fvalue;
		} else if ( tptype == "Ss" ){
			return (_Tp&) attr->svalue;
		} else if ( attr->type == TYPE_OBJECT || attr->type == TYPE_VECTOR ){
			return (_Tp&) attr->objptr;
		}
	}

	string atStr(string name, string _default="");
	int    atInt(string name, int _default=0);
	double atFloat(string name, double _default=0.0);
	bool atObj (string name, TiObj& out);

	string toString(string name);

	int is(string name);
	int has(string name);

	string encode(int tab=0, bool indent=true, bool jmpline=true);
	static int decode(TiObj& out, string text);	
};


class TiVector {
	vector<TiAttr*> itens;

public:
	int  load(string text);
	int  size();

	void add(string value);
	void add(   int value);
	void add(double value);
	void add(TiAttr* item);
	void addObject(TiObj* object);
	void addObject(string text);

	template<typename _Tp> _Tp& at(unsigned int pos){
		string tptype = typeid(_Tp).name();
		if ( pos > this->itens.size() ){
			cout << "Error{msg='Index " << pos << " is not valid'};\n";
		}

		TiAttr* attr = this->itens[pos];
		if ( tptype == "i"){
			return (_Tp&) attr->ivalue;
		} else if ( tptype == "Ss" ){
			return (_Tp&) attr->svalue;
		} else if ( tptype == "d" ){
			return (_Tp&) attr->fvalue;
		} else if ( attr->type == TYPE_OBJECT ){
			return (_Tp&) (attr->objptr);
		} else if ( attr->type == TYPE_VECTOR ){
			return (_Tp&) (attr->objptr);
		}
	}

	TiObj*  find(std::string value);

	string encode(int tab=0, bool indent=true, bool jmpline=true);
};



class TiStream {
	void* tiparser;

  public:
	TiStream();
	void open(FILE* fd);
	bool next(TiObj& obj);
};


ostream& operator<<(ostream& os, TiObj& aaa);


#endif
