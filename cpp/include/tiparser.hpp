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



/*=====================================  HEADER  ======================================*/

#pragma once

#include <iostream>
#include <stdio.h>
#include <vector>

class TiLex;
class TiParser;
class TiGlobal;

/*-------------------------------------------------------------------------------------*/



/*=====================================  TiToken  =====================================*/

class TiToken {
  public:
	enum Type {UNKNOWN, STRING, INT, DOUBLE, SYMBOL, EMPTY, END, TEXT, COMMENT, ERROR, BINARY};

	Type type;
	std::string text;
	std::string error;
	std::string aux;
	union {
		char symbol;
		long int num;
		double   dbl;
	};
	std::vector<char> bin;
};

/*-------------------------------------------------------------------------------------*/



/*====================================  TiBuffer  =====================================*/

class TiBuffer {
  protected:
	friend TiLex; friend TiParser;
	size_t line;
	bool is_good;

  public:
	unsigned char prev,last;

  public:
	inline  bool good(){return is_good;}
	virtual bool next(){}
	virtual int  readInt(){}
	virtual std::string readStr(unsigned size){return std::string();}
	virtual std::vector<char> read(size_t size){return std::vector<char>();}
};

/*-------------------------------------------------------------------------------------*/



/*==================================  TiBufferText  ===================================*/

class TiBufferText : public TiBuffer {
	std::string text;
	size_t cursor;

  public:
	TiBufferText(std::string text);
	bool next() override;
};

/*-------------------------------------------------------------------------------------*/



/*==================================  TiBufferFile  ===================================*/

class TiBufferFile : public TiBuffer {
	const static unsigned BUFFER_SIZE = 4096;

	FILE* fd;
	char buffer[TiBufferFile::BUFFER_SIZE];
	size_t cursor, size, max, already_read;


  public:
	TiBufferFile(FILE* fd);
	bool next() override ;
	int  readInt() override ;
	std::string readStr(unsigned size)  override ;
	std::vector<char> read(size_t size) override ;

  private:
	bool load();
};

/*-------------------------------------------------------------------------------------*/



/*======================================  TiLex =======================================*/

class TiLex {
	friend TiParser;
	TiBuffer* buffer;
	TiToken out;

  public:
	enum CharType {UNKNOWN, CHAR, INT, ASPA, SYMB, DEL, TEXT, COMMENT, END};
	static unsigned char symbols[256];

  public:
	void load(TiBuffer* buffer);
	bool next();

	inline size_t getLine(){return buffer->line;}
	inline bool good(){return buffer->good();}

  private:
	friend TiGlobal;
	static bool run_unknown(TiLex& lex);
	static bool run_char(TiLex& lex);
	static bool run_int(TiLex& lex);
	static bool run_aspa(TiLex& lex);
	static bool run_symb(TiLex& lex);
	static bool run_text(TiLex& lex);
	static bool run_comment(TiLex& lex);
};

/*-------------------------------------------------------------------------------------*/



/*====================================  TiEvent =======================================*/

class TiEvent{
  public:
	enum Type { ERROR, ATTR_INT, ATTR_DBL, ATTR_STR, ATTR_TEXT, ATTR_BIN, ATTR_OBJ, BOX_OBJ, OBJ_END, END };

	Type type;
	std::string attr_name;
	std::string str;
	std::string text;
	union {
		long int num;
		double   dbl;
	};
	std::vector<char> bin;
};

/*-------------------------------------------------------------------------------------*/



/*====================================  TiParser ======================================*/

class TiParser {
	TiLex lex;
	std::string mem[2];
	int mem_i, state, nivel;
	bool isEndObj;

  public:
	TiEvent out;

  public:
	void load(TiBuffer* buffer);
	bool next();
	inline bool good(){return this->lex.good();}


  private:
	friend TiGlobal;
	static bool run_error(TiParser& pr);
	static bool run_int_2(TiParser& pr);
	static bool run_dbl_2(TiParser& pr);
	static bool run_string_0(TiParser& pr);
	static bool run_string_2(TiParser& pr);
	static bool run_text_0(TiParser& pr);
	static bool run_text_2(TiParser& pr);
	static bool run_symbol_0(TiParser& pr);
	static bool run_symbol_1(TiParser& pr);
	static bool run_symbol_2(TiParser& pr);
	static bool run_symbol_3(TiParser& pr);
	static bool run_binary_2(TiParser& pr);
	static bool run_end_0(TiParser& pr);
	static bool run_end_3(TiParser& pr);
};

/*-------------------------------------------------------------------------------------*/
