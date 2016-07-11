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



/*=====================================  HEADER  =======================================*/

#include "../include/tiobj.hpp"
#include "../include/tiparser.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>

using namespace std;

bool build_tiasm( TiBuffer& buffer, _TiObj* out );

/*--------------------------------------------------------------------------------------*/



/*=====================================  TiAsm  ========================================
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



/*===================================  TiContext  ======================================*/

struct TiContext {
	std::vector< _TiObj* > stack;
	_TiObj* cur;

	TiContext(){}
	TiContext( _TiObj* obj ){this->push(obj);}

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


/*--------------------------------------------------------------------------------------*/



/*===================================  TiBuilder  ======================================*/

void tibuilder_run_error(TiParser& pr, TiContext& bu){
	if ( bu.size() > 0 ){
		bu.cur = nullptr;
		_TiObj* cur = bu.stack[0];
		cur->clear();
		cur->set("classe","Error:Syntax");
		cur->set("msg", pr.out.str );
		cur->set("line", pr.out.line );
		cur->set("cursor", pr.out.cursor );
	}
}

void tibuilder_run_attr_int(TiParser& pr, TiContext& bu){
	bu.cur->set( pr.out.attr_name, pr.out.num );
}

void tibuilder_run_attr_dbl(TiParser& pr, TiContext& bu){
	bu.cur->set( pr.out.attr_name, pr.out.dbl );
}

void tibuilder_run_attr_str(TiParser& pr, TiContext& bu){
	bu.cur->set( pr.out.attr_name, pr.out.str );
}

void tibuilder_run_attr_text(TiParser& pr, TiContext& bu){
	//bu.cur->setText( pr.out.attr_name, pr.out.str, pr.out.text );
	bu.cur->set( pr.out.attr_name, pr.out.text );
}

void tibuilder_run_attr_bin(TiParser& pr, TiContext& bu){
	bu.cur->set( pr.out.attr_name, pr.out.bin );
}

void tibuilder_run_attr_obj(TiParser& pr, TiContext& bu){
	_TiObj* novo = new _TiObj;
	novo->classe = pr.out.str;
	bu.cur->set( pr.out.attr_name, novo );
	bu.push(novo);
}

void tibuilder_run_box_obj(TiParser& pr, TiContext& bu){
	_TiObj* novo = new _TiObj;
	novo->classe = pr.out.attr_name;
	bu.cur->box += novo;
	bu.push(novo);
}

void tibuilder_run_obj_end(TiParser& pr, TiContext& bu){
	if ( bu.size() > 0 ){
		bu.pop();
	}
}

void tibuilder_run_obj_end_with_str(TiParser& pr, TiContext& bu){
	bu.cur->set( pr.out.attr_name, pr.out.str );
	if ( bu.size() > 0 ){
		bu.pop();
	}
}

void tibuilder_run_end(TiParser& pr, TiContext& bu){
	assert("Error!");
}

std::function<void(TiParser&,TiContext&)> tibuilder_run[] = {
	tibuilder_run_error,
	tibuilder_run_attr_int,
	tibuilder_run_attr_dbl,
	tibuilder_run_attr_str,
	tibuilder_run_attr_text,
	tibuilder_run_attr_bin,
	tibuilder_run_attr_obj,
	tibuilder_run_box_obj,
	tibuilder_run_obj_end,
	tibuilder_run_obj_end_with_str,
	tibuilder_run_end
};

void tibuilder (TiBuffer& buffer, _TiObj* obj){
	TiContext context(obj);
	if ( buffer.good() ){
		if ( buffer.last == '!'){
			build_tiasm( buffer, obj );
		} else {
			TiParser parser;
			parser.load( &buffer );
			while ( parser.next() ){
//cout << parser.out.type <<  << endl;
				if ( parser.out.type == TiEvent::ERROR ){
					tibuilder_run_error(parser,context);
					throw tiexception( obj->toString() );
				}
				tibuilder_run[ parser.out.type ](parser,context);
			}
		}
	}
cout << "ok!" << endl;
}

bool tibuilder_step (TiParser& parser, _TiObj* obj){
	TiContext context;
	while ( parser.next() ){
		if ( parser.out.type == TiEvent::ERROR ){
			tibuilder_run_error(parser,context);
			return false;
		}

		if ( context.size() == 0 ){
			if ( parser.out.type != TiEvent::BOX_OBJ )
				continue;
			context.push(obj);
			obj->classe = parser.out.attr_name;
		} else {
			if (
				context.size() == 1 && (
					parser.out.type == TiEvent::OBJ_END ||
					parser.out.type == TiEvent::OBJ_END_WITH_STR
				)
			){
				tibuilder_run[ parser.out.type ](parser,context);
				return true;
			}
			tibuilder_run[ parser.out.type ](parser,context);
		}
	}
	return false;
}

/*--------------------------------------------------------------------------------------*/



/*======================================================================================*/

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
		tiasm_run[ (cmd&0xFF)-'a' ](buffer, cur, cmd>>16);
	}
}

/*-------------------------------------------------------------------------------------*/
