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

/*-------------------------------------------------------------------------------------*/


TiToken::TiToken(){
}



TiBufferText::TiBufferText(std::string text){
	this->cursor = -1;
	this->line   = 1;
	this->text   = text;
	this->is_good = text.size()>0;
	this->last = '\0';
}


bool TiBufferText::next() {
	this->prev = this->last;
	if ( this->last == '\n' )
		this->line += 1;
	this->cursor += 1;
	this->is_good = this->cursor < this->text.size();
	this->last = this->is_good? this->text[ this->cursor ] : '\0';
	return this->is_good;
}











TiBufferFile::TiBufferFile(FILE* fd) {
	this->fd = fd;
	this->is_good = !feof(fd);
	this->last   = '\0';
	this->cursor = 0;
	this->size   = 0;
	this->already_read = 0;

	struct stat st;
	int error = fstat(fileno(fd), &st);
	this->max = st.st_size;
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
		if ( size > TIBUFFERFILE_SIZE ){
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
		if ( !feof(this->fd) ){
			this->size = fread(this->buffer, sizeof(size_t), TIBUFFERFILE_SIZE/sizeof(size_t), this->fd);
			if ( this->size == 0 ){
				this->size = fread(this->buffer, 1, TIBUFFERFILE_SIZE, this->fd);
			} else
				this->size *= sizeof(size_t);
			this->cursor = 0;
			if ( this->size == 0 ){
				this->is_good = false;
				return false;
			}
		} else {
			this->is_good = false;
			return false;
		}
	}
	return this->is_good;
}





void TiLex::load(TiBuffer* buffer){
	this->buffer = buffer;
}


bool TiLex::next(){
	out.type = TiToken::ERROR;
	if ( !this->buffer->good() ){
		out.type = TiToken::END;
		return false;
	}

	int type;
	// Remove char without mean as ' ', '\t', ...
	do {
		type = TiLex::symbols[ buffer->last ];

		if ( type != TiLex::DEL )
			break;
	} while ( buffer->next() );

	return tilex_run[type](*this);
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
	int i=0;
	char num[512];
	lex.out.type = TiToken::INT;
	num[i] = lex.buffer->last;
	while ( lex.buffer->next() ){
		//if ( i >= 512 ){
			// ELIMINAR O RESTO DO NUMERO ATEH O FIM DA LINHA
		//}
		i+=1;
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
			if ( c == 'n' )
				buf += '\n';
			else if ( c == 't' )
				buf += '\t';
			else if ( c == '\'' )
				buf += '\'';
			else if ( c == '\"' )
				buf += '\"';
			else if ( c == '\\' )
				buf += '\\';
			else
				buf += c;
			special = false;
		} else if ( c == '\\' ){
			special = true;
		} else if ( c == '\n' ){
			lex.out.error = "Expected a \" to close the string";
			lex.out.type = TiToken::ERROR;
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
				if ( c == 'n' )
					text += '\n';
				else if ( c == 't' )
					text += '\t';
				else if ( c == '\'' )
					text += '\'';
				else if ( c == '\"' )
					text += '\"';
				else if ( c == '\\' )
					text += '\\';
				else
					text += c;
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







/*TiParser::TiParser(){
	//this->lex.load(buffer);
}*/

void TiParser::load(TiBuffer* buffer){
	this->lex.load(buffer);
}



bool TiParser::next(){
	this->state = 0;
	while ( lex.next() ){
		bool is_complete = tiparser_run[ this->state ][ lex.out.type ](*this);
		if ( is_complete ){
			return true;
		}
		++this->state;
	}
	return false;
}



bool TiParser::run_error(TiParser& pr){
	pr.out.type = TiEvent::ERROR;
	cout <<"error\n";
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
cout <<"error\n";
//	pr.out.str = pr.lex.out.text;
	return false;
}

bool TiParser::run_text_2(TiParser& pr){
	pr.out.type = TiEvent::ATTR_TEXT;
	pr.out.text = pr.lex.out.text;
	pr.out.str = pr.lex.out.aux;
	return true;
}


bool TiParser::run_symbol_0(TiParser& pr){
	char symbol = pr.lex.out.symbol;
	if ( symbol == '{' ){
		return true;
	} if ( symbol == '\n' || symbol == ';' ){
		--pr.state;
		return false;
	}
	return true;
}

bool TiParser::run_symbol_1(TiParser& pr){
	char symbol = pr.lex.out.symbol;
	if ( symbol == '=' ){
		return false;
	} else if ( symbol == '{' ){
		//parser.output->printObj(parser.memory[0].text);
		//pr.state = parser.mem_i = 0;
	}
	return true;
}

bool TiParser::run_symbol_3(TiParser& pr){
	char symbol = pr.lex.out.symbol;
	if ( symbol == ';' ){
		pr.out.type = TiEvent::ATTR_STR;
		return true;
	} else if ( symbol == '{' ){
		pr.out.type = TiEvent::ATTR_OBJ;
		return true;
	} else if ( symbol == '\n' ){
		pr.out.type = TiEvent::ATTR_STR;
		return true;
	} else {
		pr.out.type = TiEvent::ERROR;
		return true;
	}
}


bool TiParser::run_binary_2(TiParser& pr){
	pr.out.type = TiEvent::ATTR_BIN;
	pr.out.bin  = pr.lex.out.bin;
	return true;
}



/*
int main(){
	//TiBufferText buf;
	//buf.load( "felipe gustavo bombardelli" );


	pthread_t id;
	pthread_create(&id, NULL, &terminator, NULL);
	while(1){
		FILE* fd = fopen("var_1000.ti","r");

		TiParser parser;
		parser.loadFile( fd );

		while ( parser.next() ){
			//cout << "out:" << parser.out.type << " " << parser.out.num << endl;
			//parser.out.dbl += 1.0;
		}
		fclose(fd);
		G_i += 1;
	}

	return 0;
}*/




/*============================  TiGlobal  =============================*/


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
		tiparser_run[3][TiToken::SYMBOL] = TiParser::run_symbol_3;
		tiparser_run[2][TiToken::BINARY] = TiParser::run_binary_2;
	}
};

TiGlobal global;




/*-------------------------------------------------------------------------------------*/
