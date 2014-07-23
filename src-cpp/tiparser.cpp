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
	bool  isEof;
	uint  size, max, cursor;
	char* text;
	FILE* fd;

  public:
	TiBuffer(){
		this->size   = 0;
		this->fd     = NULL;
		this->cursor = 0;
		this->text   = NULL;
	}

	TiBuffer(string text){
		this->max  = 0;
		this->text = NULL;
		this->loadText(text);
	}

	TiBuffer(string filename, uint buffersize){
		this->max  = 0;
		this->text = NULL; 
		this->loadFile(filename, buffersize);
	}

	~TiBuffer(){
		if ( text )
			free(text);
	}

	void loadFile(string filename, uint buffersize=1024){
		FILE* fd = fopen(filename.c_str(), "r");
		this->size   = 0;
		this->cursor = 0;
		if (!fd)
			return;
		this->loadFile(fd, buffersize);
	}

	void loadFile(FILE* fd, uint buffersize=1024){
		this->isEof  = false;
		this->size   = 0;
		this->cursor = 0;		
		if ( this->text ){
			if (buffersize > this->max){
				free(this->text);
				this->text = new char[buffersize];
				this->max  = buffersize;
			}
		} else {
			this->text = new char[buffersize];
			this->max  = buffersize;
		}
		this->fd = fd;
	}

	void loadText(string text){
		this->isEof  = false;
		this->fd     = NULL;
		this->cursor = 0;
		uint size = text.size()+1;
		if ( this->text ){
			if (size > this->max){
				free(this->text);
				this->text = new char[size];
			}
		} else 
			this->text = new char[size];
		this->size = size;
		this->max  = max;
		strcpy(this->text, text.c_str());
	}

	bool next(char& out){
		if ( this->cursor >= this->size ){
			if ( this->fd == NULL){
				this->isEof = true;
				return false;
			} else {
				if ( !feof(this->fd) ){
					this->size   = fread(this->text, 1, this->max, this->fd);
					this->cursor = 0;
					if ( this->size == 0 ){
						this->isEof = true;
						return false;
					}
				} else {
					this->isEof = true;
					return false;
				}
			}			
		}
		out = this->text[cursor++];
		return true;
	}

	void back(){
		if ( this->cursor > 0 ){
			this->cursor--;
		}
	}
};

/*-------------------------------------------------------------------------------------*/



/*=====================================================================================*/



class TiLex {
	char lastsymbol;
	char symbols[256];
	TiBuffer buffer;

  public:
	static const int  L_NONE = 0;
	static const int  L_CHAR = 1;
	static const int   L_INT = 2;
	static const int L_FLOAT = 3;
	static const int  L_ASPA = 4;
	static const int  L_SYMB = 5;
	static const int  L_LCMT = 6;	// Line Comment

  public:
	TiLex(){
		for (int i=0; i<256; i++){
			symbols[i] = L_NONE;
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
		symbols['=']  = L_SYMB;
		symbols[':']  = L_CHAR;
		symbols[';']  = L_SYMB;
		symbols['.']  = L_SYMB;
		symbols[',']  = L_SYMB;
		symbols['(']  = L_ASPA;
		symbols[')']  = L_ASPA;
		symbols['{']  = L_SYMB;
		symbols['}']  = L_SYMB;
		symbols['[']  = L_SYMB;
		symbols[']']  = L_SYMB;
		symbols['\''] = L_ASPA;
		symbols['\"'] = L_ASPA;
		symbols['-']  = L_INT;
		symbols['_']  = L_CHAR;
		symbols['@']  = L_CHAR;
		symbols['$']  = L_CHAR;
		symbols['&']  = L_CHAR;
		symbols['#']  = L_LCMT;
		lastsymbol = 0;
	}

	void loadFile(string filename){
		buffer.loadFile(filename);
	}

	void loadFile(FILE* fd){
		buffer.loadFile(fd);
	}

	void loadText(string text){
		buffer.loadText(text);
	}

	int next(string& out_token, int& out_type){
		out_token = "";
		out_type  = L_NONE;
		char c, aspa=0;
		int type;
		type = this->symbols[this->lastsymbol];
		if ( type == L_SYMB ){
			buffer.back();
		} else if ( type == L_CHAR ){
			buffer.back();
		}
		this->lastsymbol = 0;

		bool is_lcmt = false;
		while ( buffer.next(c) ){
			if ( is_lcmt == false ){
				type = this->symbols[c];
				if ( type == L_LCMT ){
					is_lcmt = true;
				} else if ( type != L_NONE ){
					aspa = c;
					break;
				}
			} else {
				if ( c == '\n' )
					is_lcmt = false;
			}
		}
		if ( type == L_SYMB ){
			out_token = c;
			out_type  = L_SYMB;
			return true;
		} else if ( type == L_ASPA ){
			if ( c == '(' )
				aspa = ')';
			out_type = L_CHAR;
			bool special = false;
			while ( buffer.next(c) ){
				if ( special ){
					if ( c == 'n' )
						out_token += '\n';
					else if ( c == 't' )
						out_token += '\t';
					else if ( c == '\'' )
						out_token += '\'';
					else if ( c == '\"' )
						out_token += '\"';
					else if ( c == '\\' )
						out_token += '\\';
					else
						out_token += c;
					special = false;
				} else if ( c == '\\' ){
					special = true;
				} else if ( c == aspa ){
					break;
				} else
					out_token += c;
			}
		} else if ( type == L_CHAR ){
			out_token = c;
			out_type = L_CHAR;
			while ( buffer.next(c) ){
				type = this->symbols[c];
				if ( type == L_CHAR || type == L_INT ){
					out_token += c;
				} else {
					this->lastsymbol = c;
					break;
				}
			}		
		} else if ( type == L_INT ){
			out_token = c;
			out_type = L_INT;
			while ( buffer.next(c) ){
				type = this->symbols[c];
				if ( c == '.' || c == ',' ){
					out_type   = L_FLOAT;
					out_token += '.';
				} else if ( c == 'e' || c == 'E' ){
					out_type   = L_FLOAT;
					out_token += 'e';					
				} else if ( type != L_INT ){
					this->lastsymbol = c;
					break;
				} else
					out_token += c;
			}
		}
		if ( out_type != L_NONE )
			return true;
		return false;
	}
};

// 0 = Error
// 1 = Empilha Token e vai para Next;
// 2 = Vai para Next;
// 3 = Termina o passo e reduz

char PARSER_ACTION[7][10] = {
		//  C I F  { }  [ ]  = ;
		{0, 0,0,0, 0,0, 0,0, 0,0},
		{0, 1,0,0, 3,3, 0,0, 0,0},
		{0, 0,0,0, 3,0, 0,0, 2,0},
		{0, 1,1,1, 3,0, 3,0, 0,0},
		{0, 0,0,0, 3,0, 0,0, 0,3},
		{0, 0,0,0, 0,0, 0,0, 0,3},
		{0, 0,0,0, 0,0, 0,0, 0,3}
};

char PARSER_NEXT[7][10] = {
		//  C I F  { }  [ ]  = ;
		{0, 0,0,0, 0,0, 0,0, 0,0},
		{0, 2,0,0, 1,0, 0,0, 0,0},
		{0, 0,0,0, 1,0, 0,0, 3,0},		// Attr ?
		{0, 4,5,6, 1,0, 1,0, 0,0},		// Attr = [Char,Int,Float,Obj,Vector] 
		{0, 0,0,0, 1,0, 0,0, 0,1},
		{0, 0,0,0, 0,0, 0,0, 0,1},
		{0, 0,0,0, 0,0, 0,0, 0,1}
};

char PARSER_REDUCE[7][10] = {
		//  C I F  { }  [ ]  = ;
		{0, 0,0,0, 0,0, 0,0, 0,0},
		{0, 0,0,0, 5,4, 0,0, 0,0},
		{0, 0,0,0, 6,0, 0,0, 0,0},		// Attr ?
		{0, 0,0,0, 7,0, 9,0, 0,0},		// Attr = [Char,Int,Float,Obj,Vector] 
		{0, 0,0,0, 8,0, 0,0, 0,1},
		{0, 0,0,0, 0,0, 0,0, 0,2},
		{0, 0,0,0, 0,0, 0,0, 0,3}
};



class TiParser {
	int mem_i;
	TiLex lex;
	char translate[256];
	vector<string> memory;
	std::string error_msg;

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
		memory.resize(128);
		mem_i = 0;
	}

	void loadFile(string file){
		lex.loadFile(file);
	}

	void loadFile(FILE* fd){
		lex.loadFile(fd);
	}

	void loadText(string text){
		lex.loadText(text);		
	}

	bool parse(TiObj& obj, int level=0){
		int ivalue;
		double fvalue;
		this->error_msg = "";

		do {
			int reduce = step();
			if ( reduce == 0 ){
				return false;
			} else if ( reduce == 1 ){
				if ( memory[mem_i-2] == "class" ){
					obj.classe = memory[mem_i-1];
				} else {
					obj.set(memory[mem_i-2], memory[mem_i-1]);
				}
				this->memPop(2);
			} else if ( reduce == 2 ){
				if ( memory[mem_i-1] != "class" ){
					ivalue = atoi(memory[mem_i-1].c_str());
					obj.set(memory[mem_i-2], ivalue);
				}
				this->memPop(2);
			} else if ( reduce == 3 ){
				if ( memory[mem_i-1] != "class" ){
					fvalue = (float) atof(memory[mem_i-1].c_str());
					obj.set(memory[mem_i-2], fvalue);
				}
				this->memPop(2);
			} else if ( reduce == 4 ){
				break;
			} else if ( reduce == 5 ){
				TiObj* novo = new TiObj();
				if ( !this->parse(*novo, level+1) )
					return false;
				obj.addObject(novo);
			} else if ( reduce == 6 ){
				TiObj* novo = new TiObj();
				if ( !this->parse(*novo, level+1) )
					return false;
				novo->classe = memory[mem_i-1];
				obj.addObject(novo);
				this->memPop(1);
			} else if ( reduce == 7 ){
				TiObj* novo = new TiObj();
				if ( !this->parse(*novo, level+1) )
					return false;
				if ( memory[mem_i-2] != "class" ){
					obj.set(memory[mem_i-1], *novo);
				}
				this->memPop(1);
			} else if ( reduce == 8 ){
				TiObj* novo = new TiObj();
				if ( !this->parse(*novo, level+1) )
					return false;
				novo->classe = memory[mem_i-1]; 
				if ( memory[mem_i-2] != "class" ){
					obj.set(memory[mem_i-2], *novo);
				}
				this->memPop(2);
			} else if ( reduce == 9 ){
				TiVector* vetor = this->parseVector();
				if ( memory[mem_i-1] != "class" ){
					obj.set(memory[mem_i-1], *vetor);
				}
				this->memPop(1);
			}
		} while (true);
		return true;
	}


	int step(){
		int ivalue, type, csy, act, state=1;
		string token;
		if ( !lex.next(token, type) )
			return 0;
		do {
			// Next state
			csy = type;
			if ( csy == TiLex::L_SYMB ){
				csy = this->translate[token[0]];
			}

			act = PARSER_ACTION[state][csy];
			if ( act == 0 ){
				error_msg = "ERROR {msg='Expected a [";
				if ( PARSER_ACTION[state][1] != 0 ){
					error_msg += "String;";
				} else if ( PARSER_ACTION[state][2] != 0 ){
					error_msg += "Integer;";
				} else if ( PARSER_ACTION[state][3] != 0 ){
					error_msg += "Float;";
				} else if ( PARSER_ACTION[state][4] != 0 ){
					error_msg += "'{';";
				} else if ( PARSER_ACTION[state][5] != 0 ){
					error_msg += "'}';";
				} else if ( PARSER_ACTION[state][6] != 0 ){
					error_msg += "'[';";
				} else if ( PARSER_ACTION[state][7] != 0 ){
					error_msg += "'[';";
				} else if ( PARSER_ACTION[state][8] != 0 ){
					error_msg += "'=';";
				} else if ( PARSER_ACTION[state][9] != 0 ){
					error_msg += "';';";
				}
				error_msg += "';";
				return false;
			} else if ( act == 1 ){
				this->memPush(token);
			} else if ( act == 2 ){
				// nao faz nada;
			} else if ( act == 3 ){
				char reduce = PARSER_REDUCE[state][csy];
				return reduce;
			}
			state = PARSER_NEXT[state][csy];
		} while ( lex.next(token, type) );
		if ( state != 1 ){
			error_msg = "ERROR! Ainda existe item na Pilha\n";
			return 0;
		}

	}
 

  private:
	void memPush(string str){
		memory[mem_i++] = str;
	}

	void memPop(int size){
		mem_i -= size;
	}

	void memWrite(){
		cout << mem_i << endl;
		for (int i=0; i<mem_i; i++){
			cout << memory[i] << endl;
		}
	}

	TiVector* parseVector(){
		string token; int type;
		TiVector* vetor = new TiVector();
		while ( lex.next(token, type) ){
			if ( token == "]" ){
				return vetor;
			} else if ( token == "[" ){
				/*TiVector* novo = this->parseVector();
				if ( novo == NULL ){
					// Mensagem de ERRO
				}
				vetor->add(novo);*/
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
};



/*-------------------------------------------------------------------------------------*/















//int main(){
	/*TiObj obj, tmp;
	TiParser parser;
	parser.loadFile("teste1.ti");

	parser.parse(obj);
	cout << obj.encode() << endl;*/

	//obj.select(tmp, "Noun", "");
	//cout << tmp.encode();

	/*parser.addExpr("var", "[A-Za-z][A-Za-z0-9]*");
	parser.addExpr("var", "[A-Za-z][A-Za-z0-9]*o");
	parser.addExpr("int", "[0-9]*");
	parser.addExpr("float", "[0-9]+.[0-9]*");*/

	/*char c;
	Buffer buffer("texto.txt", 32);

	while ( buffer.next(c) ){
		cout << c;
	}
	cout << endl;

	buffer.loadText("Opaaa Felipe\n");
	while ( buffer.next(c) ){
		cout << c;
	}*/

//}



