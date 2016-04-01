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


/*=====================================================================================*/

#pragma once

#include <string>
#include <sys/types.h>     // include type uint
#include "titextasm.hpp"

extern "C" bool parseFile(std::string& out, std::string filename);
extern "C" bool parseFileFd(std::string& out, FILE* fd);
extern "C" bool parseText(std::string& out, std::string text);

/*-------------------------------------------------------------------------------------*/



/*=====================================================================================*/

class TiBuffer {
  public:
	bool  isEof, isClose;
	uint  size, max, cursor, line;
	char* text;
	FILE* fd;

  public:
	TiBuffer();
	TiBuffer(std::string text);
	~TiBuffer();

	void loadFile(FILE* fd, uint buffersize=8192);
	void loadText(std::string text);

	inline bool next(unsigned char& out){
		bool c = this->read(out);
		this->accept();
		return c;
	}

	inline bool read(unsigned char& out){
		if ( !isEof ){
			out = this->text[cursor];
			return true;
		} else {
			out = '\0';
			return false;
		}
	}

	inline void accept(){
		if ( this->text[cursor] == '\n' )
			this->line += 1;
		this->cursor += 1;
		this->load();
	}

private:
	void load();
};

/*-------------------------------------------------------------------------------------*/



/*=====================================================================================*/

class TiToken {
  public:
	static const int  UNKNOWN = 0;
	static const int   STRING = 1;
	static const int      INT = 2;
	static const int   DOUBLE = 3;
	static const int   SYMBOL = 4;
	static const int    EMPTY = 6;
	static const int     TEOF = 7;
	static const int     TEXT = 8;
	static const int  COMMENT = 9;
	static const int    ERROR = 10;

	int    type;
	std::string text;
	std::string error;
	std::string aux;

	TiToken();

	inline void operator=(TiToken token){
		// Keep text="" and text+=token, because only text=token the program executes a new malloc
		this->text  = "";
		this->text += token.text;
		this->type  = token.type;
	}

	std::string write();
};

/*-------------------------------------------------------------------------------------*/



/*=====================================================================================*/

class TiLex {
	unsigned int  line;
	unsigned char lastsymbol;

	bool (*runpkg[9])(TiLex& obj, TiToken& out, unsigned char ini);

  public:
	unsigned char symbols[256];
	TiBuffer buffer;

	static const int  L_UNKNOWN = 0;
	static const int     L_CHAR = 1;
	static const int      L_INT = 2;
	static const int     L_ASPA = 3;
	static const int     L_SYMB = 4;
	static const int      L_DEL = 5;	// Delete
	static const int     L_TEXT = 6;
	static const int     L_LCMT = 7;	// Comment Line
	static const int      L_EOF = 8;

  public:
	TiLex();

	inline void loadFile(FILE* fd){
		buffer.loadFile(fd);
	}

	inline void loadText(std::string text){
		buffer.loadText(text);
	}

	bool next(TiToken& out);

	inline unsigned int getLine(){
		return buffer.line;
	}

private:
	static bool run_unknown(TiLex& obj, TiToken& out, unsigned char ini);
	static bool run_symb(TiLex& obj, TiToken& out, unsigned char ini);
	static bool run_char(TiLex& obj, TiToken& out, unsigned char ini);
	static bool run_aspa(TiLex& obj, TiToken& out, unsigned char ini);
	static bool run_int(TiLex& obj, TiToken& out, unsigned char ini);
	static bool run_none(TiLex& obj, TiToken& out, unsigned char ini);
	static bool run_text(TiLex& obj, TiToken& out, unsigned char ini);
	static bool run_lcmt(TiLex& obj, TiToken& out, unsigned char ini);
};

/*-------------------------------------------------------------------------------------*/



/*=====================================================================================*/

class TiParser {
	TiLex lex;
	char translate[256];
	TiToken memory[2];
	int mem_i, state, nivel;
	bool isEndObj;

  public:
	TiTextAsm* output;


   public: TiParser();
  private: TiParser(TiParser& up_parser);

  public:
	void parseFile(std::string filename);

	inline void loadFile(FILE* fd){
		lex.loadFile(fd);
	}

	inline void loadText(std::string text){
		lex.loadText(text);
	}

	bool parse();
	bool parseStream();

  private:
	void init();
	void error(std::string msg);
	static bool run_pass_0(TiParser& parser, TiToken& token);
	static bool run_pass_1(TiParser& parser, TiToken& token);
	static bool run_pass_2(TiParser& parser, TiToken& token);
	static bool run_pass_3(TiParser& parser, TiToken& token);

	//TiVet* parseVector();
};

/*-------------------------------------------------------------------------------------*/


