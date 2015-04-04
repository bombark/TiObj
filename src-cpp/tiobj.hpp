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


#ifndef TIOBJ_HPP
#define TIOBJ_HPP



/*=====================================- HEADER -======================================*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

using namespace std;

class TiObj;
class TiVet;

/*-------------------------------------------------------------------------------------*/




/*======================================- TiVar -======================================*/

class TiVar {
  public:
	int type;
	enum Type { NULO, EMPTY, STR, INT, DBL, OBJ, VET };


	int count_ref;
	std::string   name;
	std::string   str;
	union {
		double    dbl;
		long int  num;
		TiObj*    objptr;
		TiVet*    vetptr;
	};
	char strtype[64];
	
	TiVar();
	TiVar(int isReadOnly);
	TiVar(string name);
	~TiVar();

	inline bool isNull() {return this->type == TiVar::NULO;}
	inline bool isEmpty(){return this->type == TiVar::EMPTY;}
	inline bool isStr()  {return this->type == TiVar::STR;}
	inline bool isDbl()  {return this->type == TiVar::DBL;}
	inline bool isInt()  {return this->type == TiVar::INT;}
	inline bool isObj()  {return this->type == TiVar::OBJ;}
	inline bool isVet()  {return this->type == TiVar::VET;}


	std::string      atStr();
	inline long int  atInt(){return this->num;}
	inline double    atDbl(){return this->dbl;}
	inline TiObj&    atObj(){return *this->objptr;}
	inline TiVet&    atVet(){return *this->vetptr;}

	void operator=(string value);
//	void operator=(int value);
	void operator=(long int value);
	void operator=(double value);
	void operator=(TiObj& obj);
	void operator=(TiObj* obj);
	void operator=(TiVet& vector);
	void operator=(TiVar& attr);

	string toString();
	string encode(int tab=0);

	static TiVar ObjNull;


  private:

	inline void removeObject();
};

/*-------------------------------------------------------------------------------------*/



/*======================================- TiObj -======================================*/

class TiBox : public std::vector<TiObj*> {
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
	int          last_id;
	std::string  last_name;

  public:
	int  count_ref;
	std::string classe;
	vector<TiVar>  varpkg;
	TiBox box;

	TiObj();
	TiObj(std::string text);
	~TiObj();

	void clear();
	int  loadText(std::string text);
	int  loadFile(FILE*  fd);
	int  loadFile(std::string filename);
	int  saveFile(std::string filename);

	int  loadStream(FILE* fd);
	int  loadStream(std::string filename);

	TiVar& at(string name);
	void set(std::string name, std::string value);
	void set(std::string name, int value);
	void set(std::string name, long int value);
	void set(std::string name, double value);
	void set(std::string name, TiVet& value);
	void set(std::string name, TiObj& value);
	void set(std::string name, TiObj* value);
	void setObject(std::string name, std::string text);
	void setVector(std::string name, std::string text);
	void set(TiVar& attr);
	void setText(std::string name, std::string strtype, std::string text);



	std::string atStr (std::string name, string   _default="");
	long int    atInt (std::string name, long int _default=0);
	double      atDbl (std::string name, double   _default=0.0);
	TiObj&      atObj (std::string name);

	std::string toString();
	std::string toString(std::string name);




	       bool   has(std::string name);
	inline bool isset(std::string name){this->has(name);}
	//FAZER: unset(std::string name);


	inline bool isEmpty(){return this->varpkg.size()==0 && this->box.size()==0;}
	inline unsigned length(){return this->varpkg.size();}
	bool is(std::string name);
	bool isOnly(std::string name);



	       TiObj& select(TiObj& out, std::string classes);
	inline TiObj& select(std::string classes){return this->select(*this, classes);}

	TiObj& orderby(TiObj& out, std::string field);
	TiObj& orderby(std::string field);

	//TiObj& where(TiObj& out, std::string condpkg);
	//TiObj& where(std::string condpkg);

	void groupby(TiObj& out, std::string field);


	std::string encode(int tab=0, bool indent=true, bool jmpline=true);
	static int  decode(TiObj& out, string text);


	inline TiVar& operator[](std::string name){return this->at(name);}
	inline TiVar& operator[](int i){return this->varpkg[i];}

	static TiObj ObjNull; 

};

/*-------------------------------------------------------------------------------------*/



/*======================================- TiVet -======================================*/

class TiVet {
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

/*-------------------------------------------------------------------------------------*/



/*=====================================- Others -======================================*/

ostream& operator<<(ostream& os, TiObj& obj);
ostream& operator<<(ostream& os, TiVar& var);
ostream& operator<<(ostream& os, TiBox& box);

/*-------------------------------------------------------------------------------------*/

#endif
