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


/*===================================== INCLUDE =======================================*/

#include "../include/tiasm.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;


void addslashes(std::string& out, std::string& src){
	out = "";
	for (size_t i=0;i<src.size(); i++){
		if ( src[i] == '\n' ){
			out += "\\n";
		} else if ( src[i] == '\t' ){
			out += "\\t";
		} else
			out += src[i];
	}
}

/*-------------------------------------------------------------------------------------*/


/*===================================== Lib - API =====================================*/

bool parseFile(std::string& out, std::string filename){
	FILE* fd = fopen(filename.c_str(),"r");
	if ( !fd ){
		return false;
	}
	bool res = parseFileFd(out, fd);
	fclose(fd);
	return res;
}

bool parseFileFd(std::string& out, FILE* fd){
	TiParser parser;
	parser.loadFile(fd);
	parser.parse();
	out = parser.output.text;
	return true;
}

bool parseText(string& out, string text){
	TiParser parser;
	parser.loadText(text);
	parser.parse();
	out = parser.output.text;
	return true;
}

/*--------------------------------------------------------------------------------------*/



/*===================================== TiBuffer ======================================*/

TiBuffer::TiBuffer(){
	this->size    = 0;
	this->fd      = NULL;
	this->cursor  = 0;
	this->text    = NULL;
	this->isClose = false;
	this->isEof   = true;
}

TiBuffer::TiBuffer(std::string text){
	this->max  = 0;
	this->text = NULL;
	this->loadText(text);
	this->isClose = false;
	this->isEof   = true;
}


TiBuffer::~TiBuffer(){
	if ( this->max > 0 && this->text != NULL)
		free(this->text);
	if ( this->isClose )
		fclose(fd);
}

void TiBuffer::loadFile(FILE* fd, uint buffersize){
	this->isClose = false;
	this->isEof  = false;
	this->size   = 0;
	this->cursor = 0;
	if ( this->text ){
		if (buffersize > this->max){
			free(this->text);
			this->text = (char*) malloc(buffersize);
			this->max  = buffersize;
		}
	} else {
		this->text = (char*) malloc(buffersize);
		this->max  = buffersize;
	}
	this->fd = fd;
	this->load();
	this->line = 1;
}

void TiBuffer::loadText(std::string text){
	this->isClose = false;
	this->fd     = NULL;
	this->cursor = 0;
	if ( text.size() == 0 ){
		this->isEof  = true;
		this->size   = 0;
		return;
	}
	this->isEof  = false;
	uint size = text.size();
	if ( this->text ){
		if (size > this->max){
			free(this->text);
			this->text = (char*) malloc(size);
		}
	} else {
		this->text = (char*) malloc(size);
	}
	this->size = size;
	this->max  = size;
	strncpy(this->text, text.c_str(), size);
	this->line = 1;
}


void TiBuffer::load(){
	if ( this->cursor >= this->size ){
		if ( this->fd == NULL){
			this->isEof = true;
			return;
		} else {
			if ( !feof(this->fd) ){
				this->size   = fread(this->text, 1, this->max, this->fd);
				this->cursor = 0;
				if ( this->size == 0 ){
					this->isEof = true;
					return;
				}
			} else {
				this->isEof = true;
				return;
			}
		}
	}
}


/*-------------------------------------------------------------------------------------*/



/*====================================== TiToken ======================================*/


TiToken::TiToken(){
	this->text.reserve(1024);
	this->error.reserve(128);
	this->aux.reserve(1024);
}


std::string TiToken::write(){
	string res, aux;
	if ( type == TiToken::UNKNOWN )
		res = "UNKNOWN";
	else if ( type == TiToken::STRING )
		res = "STRING ";
	else if ( type == TiToken::INT )
		res = "INT    ";
	else if ( type == TiToken::DOUBLE )
		res = "DOUBLE ";
	else if ( type == TiToken::SYMBOL )
		res = "SYMBOL ";
	else if ( type == TiToken::TEOF ){
		return "EOF\n";
	} else if ( type == TiToken::TEXT ){
		res = "TEXT   ";
	} else if ( type == TiToken::COMMENT ){
		res = "COMMENT";
	} else if ( type == TiToken::ERROR ){
		res = "ERROR  ";
	}
	addslashes(aux, this->text);
	res += ": " + aux;
	return res;
}

/*-------------------------------------------------------------------------------------*/



/*====================================== TiLex ========================================*/
/* OBSERVACAO: Utilizar o caracter como unsigned char, pois ao encontrar
 * um caracter acima de 128, ele se transforma em negativo e ocasiona segfault
 *
 */

TiLex::TiLex(){
	for (int i=0; i<256; i++){
		symbols[i] = L_UNKNOWN;
	}
	for (int i='a'; i<='z'; i++){
		symbols[i] = L_CHAR;
	}
	for (int i='A'; i<='Z'; i++){
		symbols[i] = L_CHAR;
	}
	for (int i='0'; i<='9'; i++){
		symbols[i] = L_INT;
	}
	symbols['\t'] = L_DEL;
	symbols[' ']  = L_DEL;
	symbols['=']  = L_SYMB;
	symbols[':']  = L_CHAR;
	symbols[';']  = L_SYMB;
	symbols['.']  = L_SYMB;
	symbols[',']  = L_SYMB;
	symbols['(']  = L_SYMB;
	symbols[')']  = L_SYMB;
	symbols['{']  = L_SYMB;
	symbols['}']  = L_SYMB;
	symbols['[']  = L_SYMB;
	symbols[']']  = L_SYMB;
	symbols['<']  = L_TEXT;
	symbols['>']  = L_SYMB;
	symbols['#']  = L_LCMT;
	symbols['\''] = L_ASPA;
	symbols['\"'] = L_ASPA;
	symbols['-']  = L_INT;
	symbols['+']  = L_INT;
	symbols['_']  = L_CHAR;
	symbols['@']  = L_CHAR;
	symbols['$']  = L_CHAR;
	symbols['&']  = L_CHAR;
	symbols['!']  = L_CHAR;
	symbols['*']  = L_SYMB;
	symbols['\n'] = L_SYMB;
	lastsymbol = 0;
	runpkg[L_UNKNOWN] = run_unknown;
	runpkg[L_CHAR]    = run_char;
	runpkg[L_INT]     = run_int;
	runpkg[L_ASPA]    = run_aspa;
	runpkg[L_SYMB]    = run_symb;
	runpkg[L_DEL]     = run_none;
	runpkg[L_TEXT]    = run_text;
	runpkg[L_LCMT]    = run_lcmt;
	runpkg[L_EOF]     = run_none;
}


bool TiLex::next(TiToken& out){
	out.text = "";
	out.type = TiToken::EMPTY;
	unsigned char c, c1;
	int type;

	if ( this->buffer.isEof ){
		out.type = TiToken::TEOF;
		return false;
	}

	// Remove char without mean as ' ', '\t', ...
	while ( buffer.next(c) ){
		type = this->symbols[c];
		if ( type != L_DEL ){
			break;
		}
	}

	return runpkg[type](*this, out, c);
}


bool TiLex::run_unknown(TiLex& obj, TiToken& out, unsigned char ini){
	out.text = ini;
	out.type = TiToken::UNKNOWN;
	return true;
}

bool TiLex::run_symb(TiLex& obj, TiToken& out, unsigned char ini){
	out.text = ini;
	out.type = TiToken::SYMBOL;
	return true;
}

bool TiLex::run_char(TiLex& obj, TiToken& out, unsigned char ini){
	unsigned char c;
	int type;
	out.text = ini;
	out.type = TiToken::STRING;
	while ( obj.buffer.read(c) ){
		type = obj.symbols[c];
		if ( type == TiLex::L_CHAR || type == TiLex::L_INT ){
			out.text += c;
		} else {
			break;
		}
		obj.buffer.accept();
	}
	return true;
}

bool TiLex::run_aspa(TiLex& obj, TiToken& out, unsigned char ini){
	out.type = TiToken::STRING;
	bool special = false;
	unsigned char c, aspa = ini;
	while ( obj.buffer.read(c) ){
		if ( special ){
			if ( c == 'n' )
				out.text += '\n';
			else if ( c == 't' )
				out.text += '\t';
			else if ( c == '\'' )
				out.text += '\'';
			else if ( c == '\"' )
				out.text += '\"';
			else if ( c == '\\' )
				out.text += '\\';
			else
				out.text += c;
			special = false;
		} else if ( c == '\\' ){
			special = true;
		} else if ( c == '\n' ){
			out.error = "Expected a \" to close the string";
			out.type = TiToken::ERROR;
			return true;
		} else if ( c == aspa ){
			obj.buffer.accept();
			break;
		} else
			out.text += c;
		obj.buffer.accept();
	}
	return true;
}

bool TiLex::run_int(TiLex& obj, TiToken& out, unsigned char ini){
	unsigned char c;
	int type;
	out.text = ini;
	out.type = TiToken::INT;
	while ( obj.buffer.read(c) ){
		type = obj.symbols[c];
		if ( c == '.' || c == ',' ){
			out.type   = TiToken::DOUBLE;
			out.text += '.';
		} else if ( c == 'e' || c == 'E' ){
			out.type   = TiToken::DOUBLE;
			out.text += 'e';
		} else if ( type != L_INT ){
			break;
		} else
			out.text += c;
		obj.buffer.accept();
	}
	return true;
}

bool TiLex::run_none(TiLex& obj, TiToken& out, unsigned char ini){
	return false;
}

bool TiLex::run_text(TiLex& obj, TiToken& out, unsigned char ini){
	unsigned int level = 1;
	out.aux  = "";
	out.text = "";
	out.type = TiToken::TEXT;
	bool special = false;
	unsigned char c, c1;


	// Get the Text type;
	out.aux = "";
	while ( obj.buffer.next(c) ){
		if ( c == '|' ){
			break;
		} else {
			out.aux += c;
		}
	}

	uint size = atoi(out.aux.c_str());
	if ( size > 0 ){
		for(uint i=0; i<size; i++){
			obj.buffer.next(c);
			out.text += c;
		}

	} else {
		while ( obj.buffer.read(c) ){
			if ( special ){
				if ( c == 'n' )
					out.text += '\n';
				else if ( c == 't' )
					out.text += '\t';
				else if ( c == '\'' )
					out.text += '\'';
				else if ( c == '\"' )
					out.text += '\"';
				else if ( c == '\\' )
					out.text += '\\';
				else
					out.text += c;
				special = false;
				obj.buffer.accept();
			} else if ( c == '\\' ){
				special = true;
				obj.buffer.accept();

			} else if ( c == '|' ){
				obj.buffer.accept();
				obj.buffer.read(c1);
				if ( c1 == '>' ){
					obj.buffer.accept();
					level -= 1;
					if ( level == 0 )
						break;
					out.text += "|>";
				} else
					out.text += c;
			} else {
				out.text += c;
				obj.buffer.accept();
			}
		}
	}

	return true;
}

bool TiLex::run_lcmt(TiLex& obj, TiToken& out, unsigned char ini){
	unsigned char c;
	out.text = ini;
	out.type = TiToken::COMMENT;
	while ( obj.buffer.next(c) ){
		if ( c == '\n' )
			break;
		out.text += c;
	}
	return true;
}

/*-------------------------------------------------------------------------------------*/





/*=====================================  TiParser  =======================================
 * a: Attr + (4b) int
 * b: Attr + (4b) float
 * c: Attr + text
 * d: Attr + ClassName + text
 * e: Attr + new Object 
 * f: Attr + ClassName + New Object
 * g: New Object
 * h: ClassName + New Object
 * i: return Class
 * j: new Vector
 * l: return Vector
 *--------------------------------------------------------------------------------------*/


TiParser::TiParser(){
	for (int i=0; i<256; i++){
		translate[i] = 0;
	}
	translate['{'] = 4;
	translate['}'] = 5;
	translate['['] = 6;
	translate[']'] = 7;
	translate['='] = 8;
	translate[';'] = 9;
}



bool TiParser::parse(){
	// Init function array and other variables
	TiToken token;
	this->mem_i = this->state = this->nivel = 0;
	bool (*run[4])(TiParser& parser, TiToken& token);
	run[0] = run_pass_0;
	run[1] = run_pass_1;
	run[2] = run_pass_2;
	run[3] = run_pass_3;

	// Run the parser
	while ( lex.next(token) ){
		if ( token.type == TiToken::COMMENT)
			continue;
		if ( token.type == TiToken::ERROR ){
			//cout << this->state << " : " << token.write() << endl;
			this->error( token.error );
			return false;
		}

		bool ok = run[this->state](*this, token);
		//cout << this->state << " : " << token.write() << endl;
		if ( !ok ){
			return false;
		}
	}

	// Execute the strings in the buffer
	if ( this->state > 0 ){
		token.text = "\n";
		token.type = TiToken::SYMBOL;
		bool ok = run[this->state](*this, token);
		if ( !ok ){
			if ( token.type == TiToken::ERROR ){
				this->error("AQUI BB");
			}
			return false;
		}
	}

	//cout << this->output << endl;
	if ( nivel == 0 ){
		return true;
	} else if ( nivel < 0 ) {
		this->error("There are more '}' than '{'");
	} else
		this->error("There are more '{' than '}'");

	return false;
}

#include <iostream>
using namespace std;

bool TiParser::parseStream(){
	this->output.clear();

	// Init function array and other variables
	TiToken token;
	this->mem_i = this->state = this->nivel = 0;
	bool (*run[4])(TiParser& parser, TiToken& token);
	run[0] = run_pass_0;
	run[1] = run_pass_1;
	run[2] = run_pass_2;
	run[3] = run_pass_3;
	this->isEndObj = false;

	// Run the parser
	while ( lex.next(token) ){
		if ( token.type == TiToken::COMMENT)
			continue;
		if ( token.type == TiToken::ERROR ){
			//cout << this->state << " : " << token.write() << endl;
			this->error( token.error );
			return false;
		}

		bool ok = run[this->state](*this, token);
		//cout << this->state << " : " << token.write() << endl;
		if ( !ok ){
			return false;
		}

		if ( this->isEndObj ){
			return true;
		}
	}

	if ( this->output.text.size () <= 4 )
		return false;

	// Execute the strings in the buffer
	if ( this->state > 0 ){
		token.text = "\n";
		token.type = TiToken::SYMBOL;
		bool ok = run[this->state](*this, token);
		if ( !ok ){
			if ( token.type == TiToken::ERROR ){
				this->error("AQUI BB");
			}
			return false;
		}
	}

	if ( nivel == 0 ){
		return true;
	} else if ( nivel < 0 ) {
		this->error("There are more '}' than '{'");
	} else
		this->error("There are more '{' than '}'");

	return false;
}












void TiParser::error(std::string msg){
	output.clear();
	output.printStr("class","Error");
	output.printInt("line",this->lex.getLine());
	output.printStr("msg",msg);
}

bool TiParser::run_pass_0(TiParser& parser, TiToken& token){
	if ( parser.mem_i > 0 ){
		parser.error("Objects stack is corrupted");
		return false;
	}
	if ( token.type == TiToken::STRING ){
		parser.memory[0] = token;
		parser.mem_i += 1;
		parser.state  = 1;
		return true;
	} else if ( token.type==TiToken::SYMBOL ){
		char symbol = token.text[0];
		if ( symbol == '\n' || symbol == ';' ){
			return true;
		} else if ( symbol == '{' ){
			parser.nivel += 1;
			parser.output.printObj("");
		} else if ( symbol == '}' ){
			parser.nivel -= 1;
			parser.output.printRet();
			parser.isEndObj = true;
		} else {
			parser.error("Symbol not expected:"+token.text);
			return false;
		}
	} else {
		parser.error("Symbol not expected:"+token.text);
		return false;
	}

	return true;
}

bool TiParser::run_pass_1(TiParser& parser, TiToken& token){
	if ( token.type != TiToken::SYMBOL ){
		parser.error("Expected a Symbol like '=' or '{', and not "+token.text);
		return false;
	}
	char c = token.text[0];
	if ( c == '=' ){
		parser.state = 2;
	} else if ( c == '{' ){
		parser.nivel += 1;
		parser.output.printObj(parser.memory[0].text);
		parser.state = parser.mem_i = 0;
	} else {
		parser.error("Expected a Symbol like '=' or '{', and not '"+token.text+"'");
		return false;
	}

	return true;
}

bool TiParser::run_pass_2(TiParser& parser, TiToken& token){
	if ( token.type == TiToken::INT ){
		parser.output.printInt(parser.memory[0].text,atoi(token.text.c_str()));
		parser.state = parser.mem_i = 0;
	} else if ( token.type == TiToken::DOUBLE ){
		parser.output.printDbl(parser.memory[0].text,atoi(token.text.c_str()));
		parser.state = parser.mem_i = 0;
	} else if ( token.type == TiToken::STRING ){
		parser.memory[1] = token;
		parser.state = 3;
	} else if ( token.type == TiToken::TEXT ){
		parser.output.printStr(parser.memory[0].text,token.text);
		parser.state = parser.mem_i = 0;
	} else if ( token.type==TiToken::SYMBOL ){
		if ( token.text[0] == '{' ){
			parser.nivel += 1;
			parser.output.printVarObj(parser.memory[0].text, "");
			parser.state = parser.mem_i = 0;
		} else if ( token.text[0] == '[' ){
			parser.nivel = 0;
			//parser.output += 'j';
			//parser.output += ' ';
			parser.state = parser.mem_i = 0;
		} else {
			parser.error("Symbol not expected: "+token.text);
			return false;
		}
	} else if ( token.type==TiToken::ERROR ){
		// Mensagem de erro estah no LEX;
		return false;
	} else {
		parser.error("Symbol not expected: "+token.text);
		return false;
	}
	return true;
}

bool TiParser::run_pass_3(TiParser& parser, TiToken& token){
	if ( token.type == TiToken::SYMBOL ){
		if ( token.text[0] == ';' || token.text[0] == '\n' ){
			parser.output.printStr(parser.memory[0].text, parser.memory[1].text);

		} else if ( token.text[0] == '{' ){
			parser.nivel += 1;
			parser.output.printVarObj(parser.memory[0].text,parser.memory[1].text);

		} else if ( token.text[0] == '}' ){
			parser.nivel -= 1;
			parser.output.printStr(parser.memory[0].text,parser.memory[1].text);
			parser.output.printRet();
			parser.isEndObj = true;
		} else {
			parser.error("Expected a Text or a Symbol like ';','{','}', and not a '"+token.text+"'");
			return false;
		}
	} else if ( token.type == TiToken::TEXT ){
		//cur->setText(parser.memory[0].text, parser.memory[1].text, token.text );
	} else {
		parser.error("Expected a Text or a Symbol like ';','{','}', and not a '"+token.text+"'");
		return false;
	}
	parser.state = parser.mem_i = 0;
	return true;
}

/*
TiVet* TiParser::parseVector(){
	TiToken token;
	TiVet* vetor = new TiVet();
	while ( lex.next(token) ){
		if ( token.text == "]" ){
			return vetor;
		} else if ( token.text == "[" ){
			TiVet* novo = this->parseVector();
			if ( novo == NULL ){
				// Mensagem de ERRO
			}
			vetor->add(novo);
		} else if (type == TiLex::L_CHAR ){
			vetor->add(token);
		} else if (type == TiLex::L_INT ){
			vetor->add(atoi(token.c_str()));
		} else if (type == TiLex::L_FLOAT ){
			vetor->add(atof(token.c_str()));
		} else if (type == TiLex::L_SYMB ){
			continue;
		} else {
			// Mensagem de Erro
		}
	}
	return NULL;	// Colocar aviso de Erro(A pessoa esqueceu de colocar um ']')
}
*/

/*-------------------------------------------------------------------------------------*/





