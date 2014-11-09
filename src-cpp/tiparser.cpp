/*

PARA FAZER
	BUGS:
		segmentation fault quando existe uma string com " sem fechar
*/


#include <fstream>
#include <sstream>
#include <string.h>
#include <string>
#include "tiobj.hpp"

using namespace std;


/*=====================================================================================*/

#define uint unsigned int

#define TI_BUFFER_SIZE 8096

class TiBuffer {
  public:
	bool  isEof, isClose;
	uint  size, max, cursor, line;
	char* text;
	FILE* fd;

  public:
	TiBuffer(){
		this->size   = 0;
		this->fd     = NULL;
		this->cursor = 0;
		this->text   = NULL;
		this->isClose = false;
		this->isEof = true;
	}

	TiBuffer(string text){
		this->max  = 0;
		this->text = NULL;
		this->loadText(text);
		this->isClose = false;
		this->isEof = true;
	}


	~TiBuffer(){
		if ( this->max > 0 && this->text != NULL)
			free(this->text);
		if ( this->isClose )
			fclose(fd);
	}

	void loadFile(FILE* fd, uint buffersize=1024){
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

	void loadText(string text){
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
	void load(){
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
};

/*-------------------------------------------------------------------------------------*/



/*=====================================================================================*/


void addslashes(string& out, string& src){
	out = "";
	for (int i=0;i<src.size(); i++){
		if ( src[i] == '\n' ){
			out += "\\n";
		} else if ( src[i] == '\t' ){
			out += "\\t";
		} else
			out += src[i];
	}
}

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
	
	string text, error;
	int    type;
	
	inline void operator=(TiToken token){
		this->text = token.text;
		this->type = token.type;
	}
	
	std::string write(){
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
};

/* OBSERVACAO: Utilizar o caracter como unsigned char, pois ao encontrar
 * um caracter acima de 128, ele se transforma em negativo e ocasiona segfault
 * 
 */
class TiLex {
	unsigned int line;
	char lastsymbol;
	char symbols[256];
	TiBuffer buffer;
	bool (*runpkg[9])(TiLex& obj, TiToken& out, unsigned char ini);

  public:
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
	TiLex(){
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
		symbols['<']  = L_SYMB;
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
		symbols['$']  = L_SYMB;		
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

	void loadFile(FILE* fd){
		buffer.loadFile(fd);
	}

	void loadText(string text){
		buffer.loadText(text);
	}

	bool next(TiToken& out){
		out.text = "";
		out.type = TiToken::EMPTY;
		unsigned char c, c1;
		int type;

		if ( this->buffer.isEof ){
			out.type = TiToken::TEOF;
			return false;
		}
		while ( buffer.next(c) ){
			type = this->symbols[c];
			if ( type != L_DEL ){
				break;
			}
		}

		// alteração para deixar o texto iniciar com <| e terminar com |>
		buffer.read(c1);
		if ( c=='<' && c1 == '|' ){
			buffer.accept();
			return runpkg[L_TEXT](*this, out, c);
		} else
			return runpkg[type](*this, out, c);
	}
	
	inline unsigned int getLine(){
		return buffer.line;
	}
	
private:
	static bool run_unknown(TiLex& obj, TiToken& out, unsigned char ini){
		out.text = ini;
		out.type = TiToken::UNKNOWN;
		return true;
	}

	static bool run_symb(TiLex& obj, TiToken& out, unsigned char ini){
		out.text = ini;
		out.type = TiToken::SYMBOL;
		return true;
	}
	
	static bool run_char(TiLex& obj, TiToken& out, unsigned char ini){
		unsigned char c;
		int type;
		out.text = ini;
		out.type = TiToken::STRING;
		while ( obj.buffer.read(c) ){
			type = obj.symbols[c];
			if ( type == L_CHAR || type == L_INT ){
				out.text += c;
			} else {
				break;
			}
			obj.buffer.accept();
		}
		return true;
	}
	
	static bool run_aspa(TiLex& obj, TiToken& out, unsigned char ini){
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
	
	static bool run_int(TiLex& obj, TiToken& out, unsigned char ini){
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
	
	static bool run_none(TiLex& obj, TiToken& out, unsigned char ini){
		return false;
	}
	
	static bool run_text(TiLex& obj, TiToken& out, unsigned char ini){
		unsigned int level = 1;
		out.text = "";
		out.type = TiToken::TEXT;
		bool special = false;
		unsigned char c, c1;
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
			} else if ( c == '<' ){
				obj.buffer.accept();
				obj.buffer.read(c1);
				if ( c1 == '|' ){
					out.text += "<|";
					level += 1;
					obj.buffer.accept();
				}
			} else if ( c == '|' ){
				obj.buffer.accept();
				obj.buffer.read(c1);
				if ( c1 == '>' ){
					obj.buffer.accept();
					level -= 1;
					if ( level == 0 )
						break;
					out.text += "|>";
				}
			} else {
				out.text += c;
				obj.buffer.accept();
			}
		}
		if ( level > 0 ){
			out.error = "Expected a |> to close the text";
			out.type  = TiToken::ERROR;
		}
		
		return true;
	}
	
	static bool run_lcmt(TiLex& obj, TiToken& out, unsigned char ini){
		unsigned char c;
		int type;
		out.text = ini;
		out.type = TiToken::COMMENT;
		while ( obj.buffer.next(c) ){
			if ( c == '\n' )
				break;
			out.text += c;
		}
		return true;
	}
};





int STATE;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class TiParser {
	TiLex lex;
	char translate[256];
	vector<TiObj*> objstack;
	TiToken memory[2];
	int mem_i, state;
	TiObj* base;
	
  public:
	TiParser(){
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

	~TiParser(){
	}

	void loadFile(FILE* fd){
		lex.loadFile(fd);
	}

	void loadText(string text){
		lex.loadText(text);
	}

	bool parse(TiObj& obj){
		// Init function array and other variables
		this->base = &obj;
		this->objstack.clear();
		this->objstack.push_back(base);
		TiToken token;
		this->mem_i = this->state = 0;
		bool (*run[4])(TiParser& obj, TiToken& token);
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
		
		if ( this->objstack.size() != 1 ){
			this->error("Expected a '}'");
			return false;
		}
		
		return true;
		
	}
	

	
	
	
private:
	void error(string msg){
		this->base->clear();
		this->base->classe="ERROR";
		this->base->set("msg", msg);
		this->base->set("line", (long int)this->lex.getLine());
	}
	
	static bool run_pass_0(TiParser& parser, TiToken& token){

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
				TiObj* cur = parser.objstack[ parser.objstack.size()-1 ];
				TiObj* aux = new TiObj();
				cur->box.push_back(aux);
				parser.objstack.push_back(aux);
				parser.state = parser.mem_i = 0;
			} else if ( symbol == '}' ){
				parser.objstack.pop_back();
				if ( parser.objstack.size() == 0 ){
					parser.error("Objects stack is corrupted, there is a '}' without a '{'");
					return false;
				}
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

	static bool run_pass_1(TiParser& parser, TiToken& token){
		if ( token.type != TiToken::SYMBOL ){
			parser.error("Expected a Symbol like '=' or '{', and not "+token.text);
			return false;
		}
		char c = token.text[0];
		if ( c == '=' ){
			parser.state  = 2;
		} else if ( c == '{' ){
			TiObj* cur = parser.objstack[ parser.objstack.size()-1 ];
			TiObj* aux = new TiObj();
			cur->box.push_back(aux);
			parser.objstack.push_back(aux);
			aux->classe = parser.memory[0].text;
			parser.state = parser.mem_i = 0;
		} else {
			parser.error("Expected a Symbol like '=' or '{', and not '"+token.text+"'");
			return false;
		}
		
		return true;
	}
	
	static bool run_pass_2(TiParser& parser, TiToken& token){
		TiObj* cur = parser.objstack[ parser.objstack.size()-1 ];
		if ( token.type == TiToken::INT ){
			cur->set(parser.memory[0].text, (long int) atoi(token.text.c_str()) );
			parser.state = parser.mem_i = 0;
		} else if ( token.type == TiToken::DOUBLE ){
			cur->set(parser.memory[0].text, atof(token.text.c_str()) );
			parser.state = parser.mem_i = 0;
		} else if ( token.type == TiToken::STRING ){
			parser.memory[1] = token;
			parser.state = 3;
		} else if ( token.type == TiToken::TEXT ){
			cur->setText(parser.memory[0].text, "", token.text );
			parser.state = parser.mem_i = 0;
		} else if ( token.type==TiToken::SYMBOL && token.text[0] == '{' ){
			TiObj* cur = parser.objstack[ parser.objstack.size()-1 ];
			TiObj* aux = new TiObj();
			parser.objstack.push_back(aux);
			cur->set(parser.memory[0].text, *aux);
			parser.state = parser.mem_i = 0;
		} else if ( token.type==TiToken::ERROR ){
			// Mensagem de erro estah no LEX;
			return false;
		} else {
			parser.error("Error(run_pass_2) : "+token.text);
			return false;
		}
		return true;
	}
	
	static bool run_pass_3(TiParser& parser, TiToken& token){
		TiObj* cur = parser.objstack[ parser.objstack.size()-1 ];

		if ( token.type == TiToken::SYMBOL ){
			if ( token.text[0] == ';' || token.text[0] == '\n' ){			
				cur->set(parser.memory[0].text, parser.memory[1].text );
				parser.state = parser.mem_i = 0;
			} else if ( token.text[0] == '{' ){
				TiObj* cur = parser.objstack[ parser.objstack.size()-1 ];
				TiObj* aux = new TiObj();
				parser.objstack.push_back(aux);
				cur->set(parser.memory[0].text, *aux);
				aux->classe = parser.memory[1].text;
				parser.state = parser.mem_i = 0;
			} else if ( token.text[0] == '}' ){
				cur->set(parser.memory[0].text, parser.memory[1].text );
				parser.state = parser.mem_i = 0;
				parser.objstack.pop_back();
				if ( parser.objstack.size() == 0 ){
					parser.error("Objects stack is corrupted, there is a '}' without a '{'");
					return false;
				}
			} else {
				parser.error("Expected a Text or a Symbol like ';','{','}', and not a '"+token.text+"'");
				return false;
			}
		} else if ( token.type == TiToken::TEXT ){
			cur->setText(parser.memory[0].text, parser.memory[1].text, token.text );
			parser.state = parser.mem_i = 0;
		} else {
			parser.error("Expected a Text or a Symbol like ';','{','}', and not a '"+token.text+"'");
			return false;
		}
		return true;
	}

	TiVector* parseVector(){
		/*TiToken token;
		TiVector* vetor = new TiVector();
		while ( lex.next(token) ){
			if ( token.text == "]" ){
				return vetor;
			} else if ( token.text == "[" ){
				/*TiVector* novo = this->parseVector();
				if ( novo == NULL ){
					// Mensagem de ERRO
				}
				vetor->add(novo);*/
		/*	} else if (type == TiLex::L_CHAR ){
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
		return NULL;	// Colocar aviso de Erro(A pessoa esqueceu de colocar um ']')*/
	}
};



/*-------------------------------------------------------------------------------------*/








