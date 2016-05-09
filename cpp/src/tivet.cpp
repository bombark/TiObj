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


/*=====================================================================================*/

#include "../include/tiobj.hpp"
#include <iostream>
#include <cassert>
#include <string.h>

using namespace std;

/*-------------------------------------------------------------------------------------*/


/*=====================================================================================*/

TiVet::TiVet(){
}

TiVet::~TiVet(){
	//this->clear();
}



void TiVet::clear(){
	for (size_t i=0; i<this->data.size(); i++){
		delete this->data[i];
	}
	this->data.clear();
	this->index.clear();
}



TiVar& TiVet::operator[](std::string name){
	auto it = this->index.find ( name );
	if (it != this->index.end()){
		return *it->second;
	} else {
		return TiVar::ObjNull;
	}
}


std::string TiVet::encode(int tab, bool indent, bool jmpline){
	std::string res;
	/*res = "[";
	if ( _size > 0 ){
		res += this->data[ this->order[0] ].toString();
		for (int i=1; i<_size; i++){
			res += ';';
			res += this->data[ this->order[i] ].toString();
		}
	}
	res += "]";*/
	return res;
}


/*bool TiVet::search(std::string query, TiVar** out_obj, uint* out_addpos){
	uint ini = 0;
	uint end = _size;
	if ( query == "" ){
		*out_addpos = _size;
		*out_obj = NULL;
		return false;
	}

	while ( ini<end ){
		uint mid = (ini+end)/2;
		TiVar& cur = (*this)[ this->order[mid] ];
		int cnd = strcmp(query.c_str(), cur.name.c_str());
		if ( cnd == 0 ){
			*out_obj = &cur;
			*out_addpos = 0;
			return true;
		}
		else if ( cnd < 0 )
			end = mid;
		else
			ini = mid+1;
	}
	*out_obj = NULL;
	*out_addpos = end;
	return false;
}*/


/*-------------------------------------------------------------------------------------*/
