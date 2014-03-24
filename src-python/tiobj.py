

#=======================================================================================#

class TiBuffer:

	def __init__(self){
		self.size   = 0;
		self.cursor = 0;
		self.text   = "";
	}

	def loadFile(self, filename):
		


	def loadText(self, text):
		


	def next(out):
		


	def back():
		if ( self.cursor > 0 ):
			self.cursor--;

#---------------------------------------------------------------------------------------#



#=======================================================================================#



class TiLex:
	#char lastsymbol;
	#char symbols[256];
	#TiBuffer buffer;

	L_NONE = 0;
	L_CHAR = 1;
	L_INT = 2;
	L_FLOAT = 3;
	L_ASPA = 4;
	L_SYMB = 5;

	def __init__(self){
		self.buffer = TiBuffer();

		for (i=0; i<256; i++){
			self.symbols[i] = L_NONE;
		}
		for (i='a'; i<='z'; i++){
			self.symbols[i] = L_CHAR;
		}
		for (i='A'; i<='Z'; i++){
			self.symbols[i] = L_CHAR;
		}
		for (i='0'; i<='9'; i++){
			self.symbols[i] = L_INT;
		}
		self.symbols['=']  = L_SYMB;
		self.symbols[':']  = L_CHAR;
		self.symbols[';']  = L_SYMB;
		self.symbols['.']  = L_SYMB;
		self.symbols[',']  = L_SYMB;
		self.symbols['(']  = L_SYMB;
		self.symbols[')']  = L_SYMB;
		self.symbols['{']  = L_SYMB;
		self.symbols['}']  = L_SYMB;
		self.symbols['[']  = L_SYMB;
		self.symbols[']']  = L_SYMB;
		self.symbols['\''] = L_ASPA;
		self.symbols['\"'] = L_ASPA;
		self.symbols['-']  = L_INT;
		self.symbols['_']  = L_CHAR;
		self.symbols['@']  = L_CHAR;
		self.symbols['$']  = L_CHAR;
		self.symbols['&']  = L_CHAR;
		self.symbols['#']  = L_CHAR;
		self.lastsymbol = 0;
	}

	def loadFile(self, filename):
		buffer.loadFile(filename);
	

	def loadText(self, text):
		buffer.loadText(text);


	def next(self, out_token, out_type){
		out_token = "";
		char c, aspa=0;
		int type;
		type = this->symbols[this->lastsymbol];
		if ( type == L_SYMB ){
			buffer.back();
		} else if ( type == L_CHAR ){
			buffer.back();
		}
		this->lastsymbol = 0;

		while ( buffer.next(c) ){
			type = this->symbols[c];
			if ( type != L_NONE ){
				aspa = c;
				break;
			}
		}
		if ( type == L_SYMB ){
			out_token = c;
			out_type  = L_SYMB;
			return true;
		} else if ( type == L_ASPA ){
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
					special = false;
				} else if ( c == '\\' ){
					special = true;
				} else if ( c == aspa ){
					break;
				} else
					out_token += c;
			}
			out_type = L_CHAR;
		} else if ( type == L_CHAR ){
			out_token = c;
			while ( buffer.next(c) ){
				type = this->symbols[c];
				if ( type == L_CHAR || type == L_INT ){
					out_token += c;
				} else {
					this->lastsymbol = c;
					break;
				}
			}
			out_type = L_CHAR;
		} else if ( type == L_INT ){
			out_token = c;
			out_type = L_INT;
			while ( buffer.next(c) ){
				type = this->symbols[c];
				if ( c == '.' || c == 'e' ){
					out_type = L_FLOAT;
				} else if ( type != L_INT ){
					this->lastsymbol = c;
					break;
				}
				out_token += c;
			}
		}
		if ( out_token.size() > 0 )
			return true;
		return false;
	}
};


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

#=======================================================================================#

class TiParser {
	int mem_i;
	TiLex lex;
	char translate[256];
	vector<string> memory;

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
		do {
			int reduce = step();
			if ( reduce == 0 ){
				return false;
			} else if ( reduce == 1 ){
				obj.set(memory[mem_i-2], memory[mem_i-1]);
				this->memPop(2);
			} else if ( reduce == 2 ){
				ivalue = atoi(memory[mem_i-1].c_str());
				obj.set(memory[mem_i-2], ivalue);
				this->memPop(2);
			} else if ( reduce == 3 ){
				fvalue = atof(memory[mem_i-1].c_str());
				obj.set(memory[mem_i-2], fvalue);
				this->memPop(2);
			} else if ( reduce == 4 ){
				break;
			} else if ( reduce == 5 ){
				if ( level > 0 ){
					TiObj* novo = new TiObj();
					if ( !this->parse(*novo, level+1) )
						return false;
					obj.addObject(novo);
				} else {
					level = 1;
				}
			} else if ( reduce == 6 ){
				if ( level > 0 ){
					TiObj* novo = new TiObj();
					if ( !this->parse(*novo, level+1) )
						return false;
					novo->classe = memory[mem_i-1];
					obj.addObject(novo);
					this->memPop(1);
				} else {
					level = 1;
					obj.classe = memory[mem_i-1];
					this->memPop(1);
				}
			} else if ( reduce == 7 ){
				TiObj* novo = new TiObj();
				if ( !this->parse(*novo, level+1) )
					return false;
				obj.set(memory[mem_i-1], novo);
				this->memPop(1);
			} else if ( reduce == 8 ){
				TiObj* novo = new TiObj();
				if ( !this->parse(*novo, level+1) )
					return false;
				novo->classe = memory[mem_i-1]; 
				obj.set(memory[mem_i-2], novo);
				this->memPop(2);
			} else if ( reduce == 9 ){
				TiVector* vetor = this->parseVector();
				obj.set(memory[mem_i-1], vetor);
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
				cout << "ERROR!\n";
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
			cout << "ERROR!\n";
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



#---------------------------------------------------------------------------------------#



#=======================================================================================#

def props(cls):   
  return [i for i in cls.__dict__.keys() if i[:1] != '_']

def Vector_toString(vector):
	out = "[";
	for item in vector:
		if isinstance( item, int ):
			out += str(item)+";";
		elif isinstance( item, float ):
			out += str(item)+";";
		elif isinstance( item, str ):
			out += "'"+item+"';";
		elif isinstance( item, list ):
			out += Vector_toString(item); 
	out += "]";
	return out;

#---------------------------------------------------------------------------------------#



#=======================================================================================#

class TiObj:
	def __init__(self, obj_str=""):
		self.classe = "";
		self.box = [];

	def toString(self):
		out = "";
		if self.classe != "":
			out += self.classe;
		out += "{";
		properties = props(self);
		for property in properties:
			if property=="classe":
				continue;
			if property=="box":
				continue;
			out += property+"=";
			value = getattr(self, property);
			if isinstance( value, int ):
				out += str(value)+";";
			elif isinstance( value, float ):
				out += str(value)+";";
			elif isinstance( value, str ):
				out += "'"+value+"';";
			elif isinstance( value, list ):
				out += Vector_toString(value);
			elif isinstance( value, TiObj ):
				out += value.toString();

		for item in self.box:
			out += item.toString();
		out += "}";
		return out;

	def isA(self, classname):
		print "opa";

	def select(self):
		print "opa";

		
#---------------------------------------------------------------------------------------#
	



data = TiObj();
data.classe = "opa";
data.name = "felipe";
print data.toString();
	

