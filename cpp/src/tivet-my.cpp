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
	this->data = NULL;
	this->_size = 0;
	this->_max = 0;
}

TiVet::~TiVet(){
	if ( data ){
		for (int i=0; i<_size; i++){
			this->data[i].~TiVar();
		}
		free(this->data);
	}
}

size_t TiVet::size(){
	return this->_size;
}


void TiVet::clear(){
	if ( data ){
		for (int i=0; i<_size; i++){
			this->data[i].~TiVar();
		}
	}
	this->_size = 0;
}


TiVar* TiVet::allocVar(uint* out_pos){
	if ( _max == 0 ){
		_max = 4;
		this->data = (TiVar*) malloc( _max*sizeof(TiVar) );
		assert(this->data);
	} else if ( this->_size + 1 > this->_max ){
		_max = this->_max*2;
		this->data = (TiVar*) realloc( this->data, _max*sizeof(TiVar) );
		assert(this->data);
	}
	*out_pos = _size;
	TiVar* tmp = &this->data[_size];
	_size += 1;
	return new(tmp) TiVar();
}


TiVar& TiVet::push(std::string value, std::string name){
	TiVar* aux;
	uint order_pos, time_pos;
	bool exists = search(name, &aux, &order_pos);
	if ( exists ){
		*aux = value;
		return *aux;
	} else {
		TiVar& var = *this->allocVar(&time_pos);
		var = value;
		var.name = name;
		this->order.insert ( this->order.begin()+order_pos, time_pos );
		return var;
	}
}

TiVar& TiVet::push(long int value, std::string name){
	TiVar* aux;
	uint order_pos, time_pos;
	bool exists = search(name, &aux, &order_pos);
	if ( exists ){
		*aux = value;
		return *aux;
	} else {
		TiVar& var = *this->allocVar(&time_pos);
		var = value;
		var.name = name;
		this->order.insert ( this->order.begin()+order_pos, time_pos );
		return var;
	}
}

TiVar& TiVet::push(double value, std::string name){
	TiVar* aux;
	uint order_pos, time_pos;
	bool exists = search(name, &aux, &order_pos);
	if ( exists ){
		*aux = value;
		return *aux;
	} else {
		TiVar& var = *this->allocVar(&time_pos);
		var = value;
		var.name = name;
		this->order.insert ( this->order.begin()+order_pos, time_pos );
		return var;
	}
}

TiVar& TiVet::push(TiObj value, std::string name){
	TiVar* aux;
	uint order_pos, time_pos;
	bool exists = search(name, &aux, &order_pos);
	if ( exists ){
		*aux = value;
		return *aux;
	} else {
		TiVar& var = *this->allocVar(&time_pos);
		var = value;
		var.name = name;
		this->order.insert ( this->order.begin()+order_pos, time_pos );
		return var;
	}
}

TiVar& TiVet::push(_TiObj* value, std::string name){
	TiVar* aux;
	uint order_pos, time_pos;
	bool exists = search(name, &aux, &order_pos);
	if ( exists ){
		*aux = value;
		return *aux;
	} else {
		TiVar& var = *this->allocVar(&time_pos);
		var = value;
		var.name = name;
		this->order.insert ( this->order.begin()+order_pos, time_pos );
		return var;
	}
}


bool TiVet::search(std::string query, TiVar** out_obj, uint* out_addpos){
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
}


TiVar& TiVet::operator[](std::string name){
	uint pos;
	TiVar* aux;
	if ( this->search(name, &aux, &pos) ){
		return *aux;
	} else {
		return TiVar::ObjNull;
	}
}




std::string TiVet::encode(int tab, bool indent, bool jmpline){
	std::string res;
	res = "[";
	if ( _size > 0 ){
		res += this->data[ this->order[0] ].toString();
		for (int i=1; i<_size; i++){
			res += ';';
			res += this->data[ this->order[i] ].toString();
		}
	}
	res += "]";
	return res;
}

/*-------------------------------------------------------------------------------------*/
