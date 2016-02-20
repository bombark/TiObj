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
#include <typeinfo>
#include "titextasm.hpp"

class TiObj;
class TiVet;

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






/*======================================- TiVar -======================================*/

class TiVar {
  public:
	int type;
	int count_ref;
	enum Type { NULO, EMPTY, STR, INT, DBL, OBJ, VET };

	char strtype[32];
	std::string   name;
	std::string   str;
	union {
		double    dbl;
		long int  num;
		TiObj*    objptr;
		TiVet*    vetptr;
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


	std::string      atStr();
	inline long int  atInt(){return this->num;}
	inline double    atDbl(){return this->dbl;}
	inline TiObj&    atObj(){return *this->objptr;}
	inline TiVet&    atVet(){return *this->vetptr;}

	void operator=(std::string value);
	inline void operator=(int value){*this = (long int) value;}
	void operator=(long int value);
	void operator=(double value);
	void operator=(TiObj& obj);
	void operator=(TiObj* obj);
	void operator=(TiVet& vector);
	//void operator=(TiVar& attr);

	std::string toString();
	std::string encode(int tab=0);

	void tiasm(TiTextAsm& res);

	static TiVar ObjNull;


  private:
	inline void removeObject();
};

/*-------------------------------------------------------------------------------------*/



/*======================================- TiBox -======================================*/

class TiBox : public std::vector<TiObj*> {
	int i;

  public:
	void   clear();
	TiObj& next();
	TiObj& operator[](int id);
	void   operator+=(TiObj& obj);
	void   operator+=(TiObj* obj);
	void   operator+=(std::string objstr);
};

/*-------------------------------------------------------------------------------------*/


/*======================================- TiVet -======================================*/

class TiVet {
	TiVar*  data;
	unsigned int   _max;
	unsigned int   _size;
	std::vector<unsigned int> order;

  public:
	TiVet();
	~TiVet();

	unsigned int size();
	unsigned int max();

	void clear();

	TiVar& push(std::string value, std::string name="");
	TiVar& push(long int value, std::string name="");
	TiVar& push(double value, std::string name="");
	TiVar& push(TiObj& value, std::string name="");
	TiVar& push(TiObj* value, std::string name="");

	inline TiVar& push(int value, std::string name=""){this->push((long int) value, name);}



	inline TiVar& operator[](unsigned int i){return this->data[i];}
	TiVar& operator[](std::string name);

	inline bool has(std::string name){unsigned int pos;TiVar* obj;return this->search(name,&obj,&pos);}


	std::string encode(int tab=0, bool indent=true, bool jmpline=true);



  private:
	TiVar* allocVar(unsigned int* out_pos);
	bool search(std::string query, TiVar** out_obj, unsigned int* out_addpos);
};

/*-------------------------------------------------------------------------------------*/


/*======================================- TiObj -======================================*/

class TiObj {


  public:
	int  magic;
	int  count_ref;
	std::string classe;

	TiVet varpkg;
	TiBox box;


	TiObj();
	TiObj(std::string text);
	~TiObj();

	void clear();
	int  loadText(std::string text);
	int  loadFile(FILE*  fd);
	int  loadFile(std::string filename);
	int  saveFile(std::string filename);



	void set(std::string name, std::string value);
	void set(std::string name, int value);
	void set(std::string name, long int value);
	void set(std::string name, double value);
	void set(std::string name, TiVet& value);
	void set(std::string name, TiObj& value);
	void set(std::string name, TiObj* value);
	void setObj(std::string name, std::string text);
	void setVector(std::string name, std::string text);
	void set(TiVar& attr);
	void setText(std::string name, std::string strtype, std::string text);


	
	inline TiVar&      at(int i){return this->varpkg[i];}
	       TiVar&      at(std::string name, bool         create=false);
	
	std::string atStr (std::string name, std::string  _default="");
	long int    atInt (std::string name, long int     _default=0);
	double      atDbl (std::string name, double       _default=0.0);
	TiObj&      atObj (std::string name);

	std::string toString();
	std::string toString(std::string name);




	       bool   has(std::string name);
	inline bool isset(std::string name){return this->has(name);}
	inline bool hasnt(std::string name){return !this->has(name);}
	//FAZER: unset(std::string name);


	inline bool isEmpty(){return this->varpkg.size()==0 && this->box.size()==0;}
	inline unsigned length(){return this->varpkg.size();}
	inline unsigned size(){return this->box.size();}

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
	static int  decode(TiObj& out, std::string text);

	void tiasm(TiTextAsm& res);
	std::string tiasm();


	inline TiVar& operator[](std::string name){return this->at(name,true);}
	inline TiVar& operator[](int i){return this->varpkg[i];}

	static TiObj ObjNull; 

  private:
	TiVar& createVar(std::string name);
};

/*-------------------------------------------------------------------------------------*/



/*=====================================- Others -======================================*/

std::ostream& operator<<(std::ostream& os, TiObj& obj);
std::ostream& operator<<(std::ostream& os, TiVar& var);
std::ostream& operator<<(std::ostream& os, TiBox& box);


bool parseTiAsm(TiObj& out, std::string data);
bool parseTiAsm_vector(TiObj& out, std::vector<unsigned char>& data);
bool parseTiAsm_str(TiObj& out, const char* data, size_t total);

/*-------------------------------------------------------------------------------------*/


/*====================================- TiStream -=====================================*/


#include "tiasm.hpp"


class TiStream {
	TiParser parser;

  public:
	TiStream(FILE* fd);
	bool next(TiObj& out);
};



/*-------------------------------------------------------------------------------------*/



#endif