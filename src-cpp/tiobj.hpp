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
class TiCursor;

#define TYPE_NULL   0
#define TYPE_STRING 1
#define TYPE_INT    2
#define TYPE_FLOAT  3
#define TYPE_OBJECT 4
#define TYPE_VECTOR 5

class TiVar {
  public:
	int       type;
	string    name;
	string    svalue;
	union {
		double    fvalue;
		long int  ivalue;
		TiObj*    objptr;
		TiVector* vetptr;
	};

	TiVar();
	TiVar(string name);
	~TiVar();

	int getType();
	inline  bool isNull();
	inline  bool isString();
	inline  bool isFloat();
	inline  bool isInt();
	inline  bool isObject();
	inline  bool isVector();	


	string  Str();
	int     Int();
	inline  double    Dbl();
	inline  TiObj&    Obj();
	inline  TiVector& Vet();

	void operator=(string value);
	void operator=(int value);
	void operator=(double value);
	void operator=(TiObj& obj);
	void operator=(TiVector& vector);
	void operator=(TiVar& attr);

	string toString();
	string encode(int tab=0);

	static TiVar ObjNull;
};


class TiBox : public vector<TiObj*> {
	int i;

  public:
	TiObj& next();
	TiObj& operator[](int id);
	void   operator+=(TiObj& obj);
};

class TiObj {
  private:
	int     last_id;
	string  last_name;

  public:
	string classe;
	vector<TiVar>  varpkg;
	TiBox box;

	TiObj();
	TiObj(string text);
	~TiObj();

	void clear();
	int  loadFile(FILE*  fd);
	int  loadFile(string filename);
	int  loadStream(FILE* fd);
	int  loadStream(string filename);

	TiVar& at(string name);
	void set(string name, string value);
	void set(string name, int value);
	void set(string name, double value);
	void set(string name, TiVector& value);
	void set(string name, TiObj& value);
	void setObject(string name, string text);
	void setVector(string name, string text);
	void set(TiVar& attr);
	
	void addObject(TiObj* obj);
	void addObject(string text);
	void select(TiBox& out, string classe, string where="");
	void sort();


	string atStr (string name, string _default="");
	int    atInt (string name, int _default=0);
	double atDbl (string name, double _default=0.0);
	TiObj& atObj (string name);

	string toString(string name);

	int is(string name);
	int has(string name);

	string encode(int tab=0, bool indent=true, bool jmpline=true);
	static int decode(TiObj& out, string text);	


	TiVar& operator[](string name);
	TiObj& operator[](int id);

	static TiObj ObjNull; 

};


class TiVector {
	vector<TiVar*> itens;

public:
	int  load(string text);
	int  size();

	void add(string value);
	void add(   int value);
	void add(double value);
	void add(TiVar& item);
	void addObject(TiObj* object);
	void addObject(string text);


	TiObj*  find(std::string value);

	string encode(int tab=0, bool indent=true, bool jmpline=true);
};






ostream& operator<<(ostream& os, TiObj& obj);
ostream& operator<<(ostream& os, TiVar& var);
ostream& operator<<(ostream& os, TiBox& box);


#endif
