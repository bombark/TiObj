#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include "../include/tiobj.hpp"

using namespace std;

class Parser {
	int state;
	string buffer;

  public:
	string error;

  public:
	int getVar_state0(string& out_string, TiObj database, char c){
		if ( c == '@' ){
			this->state  = 1;
			this->buffer = "";
		} else {
			out_string += c;
		}
		return true;
	}

	int getVar_state1(string& out_string, TiObj database, char c){
		if ( isalnum(c) || c == '_' ){
			this->buffer += c;
		} else {
			if ( database->has(this->buffer) ){
				out_string += Join("'%s'").at(database->atStr(this->buffer)).ok;
			} else {
				this->error = "Field "+this->buffer+" not found in the Object";
				return false;
			}
			out_string += c;
			this->state = 0;
			this->buffer = "";
		}
		return true;
	}


	bool parse(std::string& out, TiObj database, std::string data){
		this->error = "";
		this->state = 0;
		for (int i=0; i<data.size(); i++){
			if ( this->state == 0 ){
				getVar_state0(out, database, data[i]);
			} else if (this->state == 1) {
				bool ok = getVar_state1(out, database, data[i]);
				if ( !ok )
					return false;
			}
		}
		if ( this->buffer.size()>0 ){
			if ( database->has(this->buffer) ){
				out += Join("'%s'").at(database->atStr(this->buffer)).ok;
				this->buffer = "";
			} else {
				this->error = "Field "+this->buffer+" not found in the Object";
				return false;
			}
		}
		return true;
	}

};




int main(int argc, char **argv){
	if ( argc < 2 ){
		cerr << "Syntax: " << argv[0] << " [Command Parameters ...]\n";
		return 1;
	}

	TiObj obj;
	TiStream stream(stdin);

	Parser parser;
	string cmd;

	int i=0;
	while( stream.next(obj) ){
		cmd = "";
		bool ok;
		obj["__id"] = Join("%s").at(i).ok;
		for ( unsigned c=1; c<argc; c++ ){
			ok = parser.parse(cmd,obj,argv[c]);
			if ( ok == false )
				break;
			cmd += " ";
		}

		if ( ok ){
			int e = system(cmd.c_str());
			if ( e == 0 ){
				//fprintf(stderr, "Process:Ok{\n\tcmd='%s';\n\tstatus=%d\n}\n", cmd.c_str(), e);
			} else
				fprintf(stderr, "Process:Error{\n\tcmd='%s';\n\tstatus=%d\n}\n", cmd.c_str(), e);
		} else {
			fprintf(stderr, "ERROR{msg='%s'}\n", parser.error.c_str());
		}
		i+=1;
	}


	return 0;
}
