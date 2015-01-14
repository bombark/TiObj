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
#define TYPE_TEXT   6

class TiVar {
  public:
	int       type;
	string    name;
	int       count_ref;
	string    str;
	union {
		double    dbl;
		long int  num;
		TiObj*    objptr;
		TiVector* vetptr;
	};
	char strtype[64];
	
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


	string    Str();
	long int  Int();
	double    Dbl();
	TiObj&    Obj();
	TiVector& Vet();

	void operator=(string value);
//	void operator=(int value);
	void operator=(long int value);
	void operator=(double value);
	void operator=(TiObj& obj);
	void operator=(TiVector& vector);
	void operator=(TiVar& attr);

	string toString();
	string encode(int tab=0);

	static TiVar ObjNull;
	
  private:
	inline void removeObject();
};


class TiBox : public vector<TiObj*> {
	int i;

  public:
	void   clear();
	TiObj& next();
	TiObj& operator[](int id);
	void   operator+=(TiObj& obj);
	void   operator+=(TiObj* obj);
	void   operator+=(string objstr);
};

class TiObj {
  private:
	int     last_id;
	string  last_name;

  public:
	int    count_ref;
	string classe;
	vector<TiVar>  varpkg;
	TiBox box;

	TiObj();
	TiObj(string text);
	~TiObj();

	void clear();
	int  loadText(string text);
	int  loadFile(FILE*  fd);
	int  loadFile(string filename);
	int  saveFile(string filename);

	int  loadStream(FILE* fd);
	int  loadStream(string filename);

	TiVar& at(string name);
	void set(string name, string value);
	void set(string name, int value);
	void set(string name, long int value);
	void set(string name, double value);
	void set(string name, TiVector& value);
	void set(string name, TiObj& value);
	void setObject(string name, string text);
	void setVector(string name, string text);
	void set(TiVar& attr);
	void setText(string name, string strtype, string text);
	
	void select(TiBox& out, string classe, string where="");
	void sort();


	string   atStr (string name, string _default="");
	long int atInt (string name, long int _default=0);
	double atDbl (string name, double _default=0.0);
	TiObj& atObj (string name);

	string toString();
	string toString(string name);

	bool is(string name);
	bool isOnly(string name);
	bool has(string name);
	inline bool isEmpty(){return this->varpkg.size()==0 && this->box.size()==0;}

	inline unsigned size(){return this->varpkg.size();}

	
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
	void add(long int value);
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
