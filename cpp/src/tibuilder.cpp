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
 * m: Attr + Size + Binary
 *--------------------------------------------------------------------------------------*/


/*=====================================================================================*/

#include "../include/tiobj.hpp"
#include "../include/tiparser.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;


bool build_tiasm( TiBuffer& buffer, _TiObj* out );

class TiBuilder {
  public:
	std::vector< _TiObj* > stack;
	_TiObj* cur;

	inline void push( _TiObj* obj ){
		this->stack.push_back(obj);
		this->cur = obj;
	}

	inline size_t size(){return this->stack.size();}

	inline void pop(){
		if ( this->stack.size() > 1 ){
			this->stack.pop_back();
			this->cur = this->stack[ this->stack.size()-1 ];
		}
	}
};


/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/


void tibuilder_run_error(TiParser& pr, TiBuilder& bu){
	if ( bu.size() > 0 ){
		bu.cur = nullptr;
		_TiObj* cur = bu.stack[0];
		cur->clear();
		cur->set("classe","Error:Syntax");
		cur->set("msg", "error");
		cur->set("line", 10);
	}
}

void tibuilder_run_attr_int(TiParser& pr, TiBuilder& bu){
	bu.cur->set( pr.out.attr_name, pr.out.num );
}

void tibuilder_run_attr_dbl(TiParser& pr, TiBuilder& bu){
	bu.cur->set( pr.out.attr_name, pr.out.dbl );
}

void tibuilder_run_attr_str(TiParser& pr, TiBuilder& bu){
	bu.cur->set( pr.out.attr_name, pr.out.str );
}

void tibuilder_run_attr_text(TiParser& pr, TiBuilder& bu){
	//bu.cur->setText( pr.out.attr_name, pr.out.str, pr.out.text );
	bu.cur->set( pr.out.attr_name, pr.out.text );
}

void tibuilder_run_attr_bin(TiParser& pr, TiBuilder& bu){
	bu.cur->set( pr.out.attr_name, pr.out.bin );
}

void tibuilder_run_attr_obj(TiParser& pr, TiBuilder& bu){
	_TiObj* novo = new _TiObj;
	novo->classe = pr.out.str;
	bu.cur->set( pr.out.attr_name, novo );
	bu.push(novo);
}

void tibuilder_run_box_obj(TiParser& pr, TiBuilder& bu){
	_TiObj* novo = new _TiObj;
	novo->classe = pr.out.attr_name;
	bu.cur->box += novo;
	bu.push(novo);
}

void tibuilder_run_obj_end(TiParser& pr, TiBuilder& bu){
	if ( bu.size() > 0 ){
		bu.pop();
	}
}


std::function<void(TiParser&,TiBuilder&)> tibuilder_run[] = {
	tibuilder_run_error,
	tibuilder_run_attr_int,
	tibuilder_run_attr_dbl,
	tibuilder_run_attr_str,
	tibuilder_run_attr_text,
	tibuilder_run_attr_bin,
	tibuilder_run_attr_obj,
	tibuilder_run_box_obj,
	tibuilder_run_obj_end
};



void tibuilder (TiBuffer& buffer, _TiObj* obj){
	TiBuilder builder;
	builder.push(obj);

	if ( buffer.good() ){
		if ( buffer.last == '!'){
			build_tiasm( buffer, obj );
		} else {
			TiParser parser;
			parser.load( &buffer );
			while ( parser.next() ){
				if ( parser.out.type == TiEvent::ERROR ){
					tibuilder_run_error(parser,builder);
					break;
				}
				tibuilder_run[ parser.out.type ](parser, builder);
			}
		}
	}
}



#include <sys/stat.h>

/*bool build_tiasm(_TiObj& out, std::string filename){
	string a;
	FILE *fd = fopen(filename.c_str(), "r");

	struct stat st;
	fstat(fileno(fd), &st);
	size_t size = st.st_size;
	a.resize(size);

	fread(a.c_str(), 1, size, fd);
	fclose(fd);

	return build_tiasm_str(out, data.c_str(), data.size());
}*/



void tiasm_run_a ( TiBuffer& buffer, _TiObj* cur, unsigned short attr_size ){
	std::string name = buffer.readStr(attr_size);
	long int num = buffer.readInt() << 32 + buffer.readInt();
	//cout<< name << " " << num << endl;
	cur->set(name, num);
}

void tiasm_run_b ( TiBuffer& buffer, _TiObj* cur, unsigned short attr_size ){
	/*uint size = buffer.readInt();
	std::string name = buffer.readStr(size);
	long int num = buffer.readLong();
	cur->set(name, (double) num);*/
}

void tiasm_run_cd ( TiBuffer& buffer, _TiObj* cur, unsigned short attr_size ){

	/*uint size = *((short*) &data[i+2]);
	const char* name = &data[i+4];
	i += 4 + (size+1)&0xFFFFFFFE;
	size = *((unsigned*) &data[i]);
	const char* s_val = &data[i+4];

	cur->set(name, s_val);
	i += 4 + ((size+1)&0xFFFFFFFE);*/
}



std::function<void(TiBuffer&, _TiObj*, unsigned short)> tiasm_run[] = {
	tiasm_run_a,
	tiasm_run_b,
	tiasm_run_cd,
	tiasm_run_cd
	//tiasm_run_e

};



bool build_tiasm( TiBuffer& buffer, _TiObj* out ){
	char ass[3];
	buffer.next();
	buffer.next();
	buffer.next();

	vector<_TiObj*> stack;
	_TiObj* cur = out;

	while ( buffer.good() ){
		int cmd = buffer.readInt();
		//cout << (cmd&0xFF)-'a' << endl;
		tiasm_run[ (cmd&0xFF)-'a' ](buffer, cur, cmd>>16);
	}
}


/*-------------------------------------------------------------------------------------*/

		/*} else if ( cmd == 'c' || cmd == 'd' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + (size+1)&0xFFFFFFFE;
			size = *((unsigned*) &data[i]);
			s_val = &data[i+4];

			cur->set(name, s_val);
			i += 4 + ((size+1)&0xFFFFFFFE);

		} else if ( cmd == 'e' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);

			_TiObj* novo = new _TiObj;
			cur->set(name, novo);
			stack.push_back(novo);
			cur = novo;

		} else if ( cmd == 'f' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);
			size = *((short*) &data[i]);
			cname = &data[i+2];
			i += 2 + ((size+1)&0xFFFFFFFE);
			_TiObj* novo = new _TiObj;
			cur->set(name, novo);
			stack.push_back(novo);
			cur = novo;
			cur->classe = cname;
		} else if ( cmd == 'g' ){
			_TiObj* novo = new _TiObj;
			cur->box += novo;
			stack.push_back( novo );
			cur = novo;
			i += 2;

		} else if ( cmd == 'h' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);
			_TiObj* novo = new _TiObj;
			cur->box += novo;
			stack.push_back( novo );
			cur = novo;
			cur->classe = name;

		} else if ( cmd == 'i' ){
			int stack_size = stack.size();
			if ( stack_size > 1 ){
				stack.pop_back();
				cur = stack[ stack_size-2];
			} else if ( stack_size == 1 ){
				stack.pop_back();
				cur = &out;
			} else
				cur = &out;
			i += 2;

		} else if ( cmd == 'm' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + (size+1)&0xFFFFFFFE;
			size = *((unsigned*) &data[i]);
			s_val = &data[i+4];
			cur->setBinary(name, (char*)s_val, size);
			i += 4 + ((size+1)&0xFFFFFFFE);

		} else {

			cerr << "tibuilder:error\n";
			break;
		}*/
