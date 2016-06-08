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



/*=====================================================================================


TiVarPkg::~TiVarPkg(){
	this->clear();
}


void TiVarPkg::clear(){
	for (size_t i=0; i<this->data.size(); i++){
		delete this->data[i];
	}
	this->data.clear();
	this->index.clear();
}


TiVar& TiVarPkg::operator[](std::string name){
	auto it = this->index.find ( name );
	if (it != this->index.end()){
		return *it->second;
	} else {
		return TiVar::ObjNull;
	}
}


TiVar& TiVarPkg::push(void* ptr, size_t size, std::string name){
	TiVar* res;
	auto it = this->index.find ( name );
	if (it != this->index.end()){
		res = it->second;
	} else {
		res = new TiVar();
		res->name = name;
		this->data.push_back( res );
		this->index.insert( std::pair<std::string,TiVar*>(name,res) );
	}
	
	res->setBinary(ptr, size);
	
	return *res;
}


---------------------------------------------------------------------------------------*/



/*=====================================================================================*/

TiVarPkg::TiVarPkg(){
}


void TiVarPkg::clear(){
	for (size_t i=0; i<this->size(); i++){
		delete this->at(i);
	}
}

bool TiVarPkg::search(std::string query, TiVar** out_obj, size_t& out_addpos){
	if ( this->size() == 0 || query.size() == 0 ){
		out_obj    = nullptr;
		out_addpos = 0;
		return false;
	}

	if ( query > this->last().name ){
		out_obj    = nullptr;
		out_addpos = this->size();
		return false;
	}

	size_t ini = 0;
	size_t end = this->size();
	while ( ini<end ){
		size_t mid = (ini+end)/2;
		TiVar* cur = this->at(mid);
		int cnd = strcmp(query.c_str(), cur->name.c_str());
		if ( cnd == 0 ){
			*out_obj = cur;
			out_addpos = 0;
			return true;
		}
		else if ( cnd < 0 )
			end = mid;
		else
			ini = mid+1;
	}
	*out_obj = NULL;
	out_addpos = end;
	return false;
}


TiVar& TiVarPkg::search(std::string query){
	size_t ini = 0;
	size_t end = this->size();
	if ( query.size() == 0 ){
		return TiVar::ObjNull;
	}

	while ( ini<end ){
		size_t mid = (ini+end)/2;
		TiVar* cur = this->at(mid);
		int cnd = strcmp(query.c_str(), cur->name.c_str());
		if ( cnd == 0 ){
			return *cur;
		}
		else if ( cnd < 0 )
			end = mid;
		else
			ini = mid+1;
	}
	return TiVar::ObjNull;
}

TiVar& TiVarPkg::push(void* ptr, size_t size, std::string name){
	TiVar* aux;
	size_t order_pos;
	bool exists = search(name, &aux, order_pos);
	if ( exists ){
		aux->setBinary(ptr,size);
		return *aux;
	} else {
		TiVar* var = new TiVar();
		var->setBinary(ptr,size);
		var->name = name;
		this->insert ( this->begin()+order_pos, var );
		return *var;
	}
}


/*-------------------------------------------------------------------------------------*/

