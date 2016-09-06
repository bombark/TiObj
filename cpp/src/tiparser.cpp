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

#include "../include/tiparser.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <functional>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

using namespace std;

#define QTDE_STATES 4
#define QTDE_TYPES  10

unsigned char TiLex::symbols[256];
function< bool(TiLex&) > tilex_run[8];
std::function<bool(TiParser&)> tiparser_run[QTDE_STATES][QTDE_TYPES];


const char* TITOKEN_NAMES[] = {
	"UNKNOWN", "STRING", "INT", "DOUBLE", "SYMBOL", "EMPTY", "END", "TEXT", "COMMENT", "ERROR", "BINARY"
};

/*-------------------------------------------------------------------------------------*/



/*=====================================  TiToken  =====================================*/

std::string TiToken::value(){
	string res;
	res = TITOKEN_NAMES[ this->type ];
	res += '(';
	switch (this->type) {
		case INT:    res += to_string(this->num); break;
		case DOUBLE: res += to_string(this->dbl); break;
		case SYMBOL: res += this->symbol; break;
		case END:    break;
		default:     res += this->text;
	}
	res+=')';
	return res;
	//UNKNOWN, STRING, INT, DOUBLE, SYMBOL, EMPTY, END, TEXT, COMMENT, ERROR, BINARY
}

/*-------------------------------------------------------------------------------------*/


/*==================================  TiBufferText  ===================================*/

TiBufferText::TiBufferText(std::string text){
	this->cursor  = 0;
	this->line    = 1;
	this->text    = text;
	if ( text.size()>0 ){
		this->is_good = true;
		this->last    = text[0];
	} else {
		this->is_good = false;
		this->last    = '\0';
	}
}


bool TiBufferText::next() {
	this->prev = this->last;
	if ( this->last == '\n' )
		this->line += 1;
	this->cursor += 1;
	this->is_good = this->cursor < this->text.size();
	this->last    = this->is_good ? this->text[ this->cursor ] : '\0';
	return this->is_good;
}

/*-------------------------------------------------------------------------------------*/



/*==================================  TiBufferFile  ===================================*/

TiBufferFile::TiBufferFile(){
}

TiBufferFile::TiBufferFile(FILE* fd) {
	this->load(fd);
}

void TiBufferFile::load(FILE* fd) {
	this->fd = fd;
	this->is_good = !feof(fd);
	this->last   = '\0';
	this->cursor = 0;
	this->size   = 0;
	this->already_read = 0;
	//struct stat st;
	//int error = fstat(fileno(fd), &st);
	//this->max = st.st_size;
	this->next();
}


bool TiBufferFile::next() {
	bool ret = this->load();
	this->prev = this->last;
	this->last = this->buffer[this->cursor];
	if ( this->last == '\n' )
		this->line += 1;
	this->cursor+=1;
	return ret;
}



int TiBufferFile::readInt(){
	int num;
	int size_num = sizeof(int);
	unsigned in_buffer = this->size - this->cursor;
	if ( size_num <= in_buffer ){
		num = *((int*)(this->buffer + this->cursor));
		this->cursor += size_num;
		return num;
	} else {
		if ( this->load() ){
			num = *((int*)(this->buffer + this->cursor));
			this->cursor += size_num;
			return num;
		}
		return 0;
	}
}


std::string TiBufferFile::readStr(unsigned size){
	string res;
	res.resize(size);
	unsigned in_buffer = this->size - this->cursor;
	if ( size <= in_buffer ){
		memcpy((void*)res.c_str(), this->buffer+this->cursor, size);
		this->cursor += (size+3)&0xFFFFFFFC;
	} else {
		char* dst = (char*)res.c_str();
		memcpy(dst, this->buffer+this->cursor, in_buffer);
		this->cursor += in_buffer;
		size  = (size+3)&0xFFFFFFFC - in_buffer;
		dst  += in_buffer;
		if ( size > BUFFER_SIZE ){
			fread(dst, 1, size, fd);
		} else {
			if ( this->load() ){
				memcpy(dst, this->buffer, size);
				this->cursor += size;
			}
		}
	}
	return res;
}


/*long int TiBufferFile::readLong(){
	return this->readInt() << 32 + this->readInt();
}*/

/*void TiBufferFile::read(char* buffer, size_t size){

	unsigned in_buffer = this->size - this->cursor;
cout << this->cursor << " " << in_buffer << endl;
	if ( size <= in_buffer ){
		memcpy(buffer, this->buffer+this->cursor, size);
		this->cursor += size;
	} else {
		memcpy(buffer, this->buffer+this->cursor, in_buffer);
		this->cursor += in_buffer;

		if ( size > TIBUFFERFILE_SIZE ){
			memcpy(buffer, this->buffer, in_buffer);
			size_t to_read = size - in_buffer;
			buffer += in_buffer;
			fread(buffer, 1, to_read, fd);
		} else {
			if ( this->load() ){
				this->read(buffer,size);
			}
		}
	}
}*/

std::vector<char> TiBufferFile::read(size_t size){
	vector<char> out;
	out.resize(size);
	fread(out.data(), 1, size, fd);
	return out;
}


bool TiBufferFile::load(){
	if ( this->cursor >= this->size ){
		this->is_good = false;
		this->cursor = 0;
		if ( !feof(this->fd) ){
			this->size = fread(this->buffer, 1, BUFFER_SIZE, this->fd);
			this->is_good = (this->size==0) ? false : true;
		}
	}
	return this->is_good;
}

/*-------------------------------------------------------------------------------------*/



/*======================================  TiLex  ======================================*/

void TiLex::load(TiBuffer* buffer){
	this->buffer = buffer;
}


bool TiLex::next(){
	int type;
	do {
		type = TiLex::symbols[ buffer->last ];
		if ( type != TiLex::DEL )
			break;
	} while ( buffer->next() );


	if ( this->buffer->good() ){
		return tilex_run[type](*this);
	} else {
		out.type = TiToken::END;
		return false;
	}
}


bool TiLex::run_unknown(TiLex& lex){
	lex.out.type = TiToken::UNKNOWN;
	lex.out.text = lex.buffer->last;
	return true;
}

bool TiLex::run_char(TiLex& lex){
	lex.out.type = TiToken::STRING;
	string buf;
	buf.reserve(32);
	buf.push_back( lex.buffer->last );

	while ( lex.buffer->next() ){
		char c = lex.buffer->last;
		int type = TiLex::symbols[c];
		if ( type == TiLex::CHAR || type == TiLex::INT ){
			buf.push_back(c);
		} else {
			break;
		}
	}
	lex.out.text = buf;
	return true;
}

bool TiLex::run_int(TiLex& lex){
	int i=1;
	char num[512];
	lex.out.type = TiToken::INT;
	num[0] = lex.buffer->last;
	while ( lex.buffer->next() ){
		//if ( i >= 512 ){
			// ELIMINAR O RESTO DO NUMERO ATEH O FIM DA LINHA
		//}

		char c = lex.buffer->last;
		int type = TiLex::symbols[c];
		if ( c == '.' || c == ',' ){
			lex.out.type   = TiToken::DOUBLE;
			num[i] = c;
		} else if ( (c|0x20) == 'e' ){
			lex.out.type  = TiToken::DOUBLE;
			num[i] = c;
		} else if ( type != TiLex::INT ){
			break;
		} else
			num[i] = c;
		i+=1;
	}
	num[i] = '\0';

	if ( lex.out.type == TiToken::DOUBLE ){
		lex.out.dbl = atof(num);
	} else {
		lex.out.num = atol(num);
	}
	return true;
}


bool TiLex::run_aspa(TiLex& lex){
	bool special = false;
	lex.out.type = TiToken::STRING;
	unsigned char c, aspa = lex.buffer->last;
	string buf;
	buf.reserve(32);
	while ( lex.buffer->next() ){
		c = lex.buffer->last;
		if ( special ){
			switch (c) {
				case 'n': buf += '\n'; break;
				case 't': buf += '\t'; break;
				case '\'': buf += '\''; break;
				case '\"': buf += '\"'; break;
				case '\\': buf += '\\'; break;
				default: buf += c;
			}
			special = false;
		} else if ( c == '\\' ){
			special = true;
		} else if ( c == '\n' ){
			lex.out.type = TiToken::ERROR;
			lex.out.error = "Expected a \" to close the string";
			return true;
		} else if ( c == aspa ){
			lex.buffer->next();
			break;
		} else
			buf += c;
	}
	lex.out.text = buf;
	return true;
}



bool TiLex::run_symb(TiLex& lex){
	lex.out.type   = TiToken::SYMBOL;
	lex.out.symbol = lex.buffer->last;
	lex.buffer->next();
	return true;
}


bool TiLex::run_text(TiLex& lex){
	unsigned int level = 1;
	string strtype, text;
	strtype.reserve(32);
	text.reserve(64);

	lex.out.type = TiToken::TEXT;
	bool special = false;
	unsigned char c, c1;
	// Get the Text type;
	while ( lex.buffer->next() ){
		c = lex.buffer->last;
		if ( c == '|' ){
			break;
		} else {
			strtype += c;
		}
	}

	size_t size = atoi(strtype.c_str());
	if ( size > 0 ){
		// <int:size|binary:data
		lex.out.type = TiToken::BINARY;
		lex.out.bin  = lex.buffer->read(size);
	} else {
		// <string:type|string:text|>
		lex.buffer->next();
		while ( lex.buffer->good() ){
			c = lex.buffer->last;
			if ( special ){
				switch (c) {
					case 'n' : text += '\n'; break;
					case 't' : text += '\t'; break;
					case '\'': text += '\''; break;
					case '\"': text += '\"'; break;
					case '\\': text += '\\'; break;
					default  : text += c;
				}
				special = false;
				lex.buffer->next();
			} else if ( c == '\\' ){
				special = true;
				lex.buffer->next();
			} else if ( c == '|' ){
				lex.buffer->next();
				c1 = lex.buffer->last;
				if ( c1 == '>' ){
					lex.buffer->next();
					break;
				} else
					text += c;
			} else {
				text += c;
				lex.buffer->next();
			}
		}

	}

	lex.out.text = text;
	return true;
}


bool TiLex::run_comment(TiLex& lex){
	lex.out.text.clear();
	lex.out.type = TiToken::COMMENT;
	while ( lex.buffer->next() ){
		if ( lex.buffer->last == '\n' )
			break;
	}
	return true;
}

/*-------------------------------------------------------------------------------------*/



/*====================================  TiParser  =====================================*/

void TiParser::load(TiBuffer* buffer){
	this->lex.load(buffer);
}

bool TiParser::next(){
	this->state = 0;
	while ( true ){
		lex.next();
		bool is_complete = tiparser_run[ this->state ][ lex.out.type ](*this);
		if ( is_complete ){
			return (out.type!=TiEvent::END) ? true : false ;
		}
		++this->state;
	}
}


//******* Runners *******
bool TiParser::run_error(TiParser& pr){
	pr.out.type   = TiEvent::ERROR;
	pr.out.str    = "Token ";
	pr.out.str    += pr.lex.out.value();
	pr.out.str    += " não esperado";
	pr.out.line   = pr.lex.buffer->line;
	pr.out.cursor = pr.lex.buffer->cursor;
	return true;
}

bool TiParser::run_int_2(TiParser& pr){
	pr.out.type = TiEvent::ATTR_INT;
	pr.out.num  = pr.lex.out.num;
	return true;
}

bool TiParser::run_dbl_2(TiParser& pr){
	pr.out.type = TiEvent::ATTR_DBL;
	pr.out.dbl  = pr.lex.out.dbl;
	return true;
}

bool TiParser::run_string_0(TiParser& pr){
	pr.out.attr_name = pr.lex.out.text;
	return false;
}

bool TiParser::run_string_2(TiParser& pr){
	pr.out.str = pr.lex.out.text;
	return false;
}

bool TiParser::run_text_0(TiParser& pr){
	pr.out.type = TiEvent::ERROR;
	return false;
}

bool TiParser::run_text_2(TiParser& pr){
	pr.out.type = TiEvent::ATTR_TEXT;
	pr.out.text = pr.lex.out.text;
	pr.out.str = pr.lex.out.aux;
	return true;
}

bool TiParser::run_symbol_0(TiParser& pr){
	switch ( pr.lex.out.symbol ) {
		case '{':
			pr.out.type = TiEvent::BOX_OBJ;
			pr.out.str.clear();
			return true;
		case '}':
			pr.out.type = TiEvent::OBJ_END;
			return true;
		case '\n':  case ';':
			--pr.state;
			return false;
		default:
			return TiParser::run_error(pr);
	}
}

bool TiParser::run_symbol_1(TiParser& pr){
	switch ( pr.lex.out.symbol ) {
		case '=':
			return false;
		case '{':
			pr.out.type = TiEvent::BOX_OBJ;
			return true;
		default:
			return TiParser::run_error(pr);
	}
}

bool TiParser::run_symbol_2(TiParser& pr){
	char symbol = pr.lex.out.symbol;
	if ( symbol == '{' ){
		pr.out.type = TiEvent::ATTR_OBJ;
		pr.out.str.clear();
	} else {
		return TiParser::run_error(pr);
	}
	return true;
}

bool TiParser::run_symbol_3(TiParser& pr){
	switch ( pr.lex.out.symbol ) {
		case ';': case '\n':
			pr.out.type = TiEvent::ATTR_STR;
			return true;
		case '{':
			pr.out.type = TiEvent::ATTR_OBJ;
			return true;
		case '}':
			pr.out.type = TiEvent::OBJ_END_WITH_STR;
			return true;
		default:
			return TiParser::run_error(pr);
	}
}

bool TiParser::run_binary_2(TiParser& pr){
	return true;
}

bool TiParser::run_end_0(TiParser& pr){
	pr.out.type = TiEvent::END;
	return true;
}

bool TiParser::run_end_3(TiParser& pr){
	pr.out.type = TiEvent::ATTR_STR;
	return true;
}

bool TiParser::run_comment(TiParser& pr){
	pr.state -= 1; // anular o state++ 
	return false;
}


/*-------------------------------------------------------------------------------------*/




/*====================================  TiGlobal  =====================================*/

class TiGlobal{
  public:
	TiGlobal(){
		for (int i=0; i<256; i++){
			TiLex::symbols[i] = TiLex::UNKNOWN;
		}
		for (int i='a'; i<='z'; i++){
			TiLex::symbols[i] = TiLex::CHAR;
		}
		for (int i='A'; i<='Z'; i++){
			TiLex::symbols[i] = TiLex::CHAR;
		}
		for (int i='0'; i<='9'; i++){
			TiLex::symbols[i] = TiLex::INT;
		}
		TiLex::symbols['\r'] = TiLex::DEL;
		TiLex::symbols['\t'] = TiLex::DEL;
		TiLex::symbols[' ']  = TiLex::DEL;
		TiLex::symbols['=']  = TiLex::SYMB;
		TiLex::symbols[':']  = TiLex::CHAR;
		TiLex::symbols[';']  = TiLex::SYMB;
		TiLex::symbols['.']  = TiLex::SYMB;
		TiLex::symbols[',']  = TiLex::SYMB;
		TiLex::symbols['(']  = TiLex::SYMB;
		TiLex::symbols[')']  = TiLex::SYMB;
		TiLex::symbols['{']  = TiLex::SYMB;
		TiLex::symbols['}']  = TiLex::SYMB;
		TiLex::symbols['[']  = TiLex::SYMB;
		TiLex::symbols[']']  = TiLex::SYMB;
		TiLex::symbols['<']  = TiLex::TEXT;
		TiLex::symbols['>']  = TiLex::SYMB;
		TiLex::symbols['#']  = TiLex::COMMENT;
		TiLex::symbols['\''] = TiLex::ASPA;
		TiLex::symbols['\"'] = TiLex::ASPA;
		TiLex::symbols['-']  = TiLex::INT;
		TiLex::symbols['+']  = TiLex::INT;
		TiLex::symbols['_']  = TiLex::CHAR;
		TiLex::symbols['@']  = TiLex::CHAR;
		TiLex::symbols['$']  = TiLex::CHAR;
		TiLex::symbols['&']  = TiLex::CHAR;
		TiLex::symbols['!']  = TiLex::SYMB;
		TiLex::symbols['*']  = TiLex::SYMB;
		TiLex::symbols['\n'] = TiLex::SYMB;

		tilex_run[TiLex::UNKNOWN] = TiLex::run_unknown;
		tilex_run[TiLex::CHAR]    = TiLex::run_char;
		tilex_run[TiLex::INT]     = TiLex::run_int;
		tilex_run[TiLex::ASPA]    = TiLex::run_aspa;
		tilex_run[TiLex::SYMB]    = TiLex::run_symb;
		tilex_run[TiLex::DEL]     = TiLex::run_unknown;
		tilex_run[TiLex::TEXT]    = TiLex::run_text;
		tilex_run[TiLex::COMMENT] = TiLex::run_comment;

		for (int y=0; y<QTDE_STATES; y++){
			for (int x=0; x<QTDE_TYPES; x++){
				tiparser_run[y][x] = TiParser::run_error;
			}
		}

		tiparser_run[2][TiToken::INT]    = TiParser::run_int_2;
		tiparser_run[2][TiToken::DOUBLE] = TiParser::run_dbl_2;
		tiparser_run[0][TiToken::STRING] = TiParser::run_string_0;
		tiparser_run[2][TiToken::STRING] = TiParser::run_string_2;
		tiparser_run[0][TiToken::TEXT]   = TiParser::run_text_0;
		tiparser_run[2][TiToken::TEXT]   = TiParser::run_text_2;
		tiparser_run[0][TiToken::SYMBOL] = TiParser::run_symbol_0;
		tiparser_run[1][TiToken::SYMBOL] = TiParser::run_symbol_1;
		tiparser_run[2][TiToken::SYMBOL] = TiParser::run_symbol_2;
		tiparser_run[3][TiToken::SYMBOL] = TiParser::run_symbol_3;
		tiparser_run[2][TiToken::BINARY] = TiParser::run_binary_2;
		tiparser_run[0][TiToken::END]    = TiParser::run_end_0;
		tiparser_run[3][TiToken::END]    = TiParser::run_end_3;
		
		tiparser_run[0][TiToken::COMMENT] = TiParser::run_comment;
		tiparser_run[1][TiToken::COMMENT] = TiParser::run_comment;
		tiparser_run[2][TiToken::COMMENT] = TiParser::run_comment;
		tiparser_run[3][TiToken::COMMENT] = TiParser::run_comment;
	}
};

TiGlobal global;

/*-------------------------------------------------------------------------------------*/
