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
 *--------------------------------------------------------------------------------------*/


/*=====================================================================================*/

#include "../include/tiobj.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;


/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/

bool parseTiAsm(TiObj& out, std::string data){
	return parseTiAsm_str(out, data.c_str(), data.size());
}

bool parseTiAsm_vector(TiObj& out, std::vector<unsigned char>& data){
	return parseTiAsm_str(out, (char*) data.data(), data.size());
}

bool parseTiAsm_str(TiObj& out, const char* data, size_t total){
	uint size;
	long int i_val;
	float f_val;
	const char* name;
	const char* s_val;
	const char* cname;
	uint i = 4, old_i = 0;

	vector<TiObj*> stack;
	TiObj* cur = &out;
	while ( i < total ){
		/*if ( old_i == i ){
			cerr << "error stall\n";
			break;
		}
		old_i = i;*/

		char cmd = data[i];
		//cout << cmd << i<< endl;
		if ( cmd == 'a' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);
			i_val = *((int*) &data[i]);
			cur->set(name, i_val);
			i += 4;


		} else if ( cmd == 'b' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);
			f_val = *((float*) &data[i]);
			cur->set(name, f_val);
			i += 4;

		} else if ( cmd == 'c' || cmd == 'd' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + (size+1)&0xFFFFFFFE;
			size = *((int*) &data[i]);
			s_val = &data[i+4];
			cur->set(name, s_val);
			i += 4 + ((size+1)&0xFFFFFFFE);

		} else if ( cmd == 'e' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);

			TiObj* novo = new TiObj();
			cur->set(name, novo);
			stack.push_back(novo);
			cur = novo;

		} else if ( cmd == 'f' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);

			size = *((int*) &data[i]);
			cname = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);

			TiObj* novo = new TiObj();
			cur->set(name, novo);
			stack.push_back(novo);
			cur = novo;
			cur->classe = cname;
		} else if ( cmd == 'g' ){
			TiObj* novo = new TiObj();
			cur->box += novo;
			stack.push_back( novo );
			cur = novo;
			i += 2;

		} else if ( cmd == 'h' ){
			size = *((short*) &data[i+2]);
			name = &data[i+4];
			i += 4 + ((size+1)&0xFFFFFFFE);
			TiObj* novo = new TiObj();
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

		} else {

			cout << ":error\n";
			break;
		}
	}
}

/*-------------------------------------------------------------------------------------*/
