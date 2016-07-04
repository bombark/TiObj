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


/*=====================================- HEADER -======================================*/


#include "../include/tiobj.hpp"




Join::Join(std::string format){
	this->format = &format;
	ok.reserve(256);

	size = format.size();
	for (pi=0; pi<size; pi++){
		char c = format[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		ok += c;
	}
}

Join& Join::at(std::string var){
	const char* text = format->c_str();
	char c = text[pi-1];
	if ( c == 'V' ){
		for (int i=0; i<var.size(); i++){
			c = var[i];
			if ( c == '"' )
				ok += "\\\"";
			else
				ok += c;
		}
	} else if ( c == 'v' ){
		for (int i=0; i<var.size(); i++){
			c = var[i];
			if ( c == '\'' )
				ok += " ";
			else
				ok += c;
		}
	} else
		ok += var;

	for (; pi<size; pi++){
		c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		ok += c;
	}
	return *this;
}


Join& Join::at(int var){
	char aux[64];
	sprintf(aux,"%d",var);
	ok += aux;
	const char* text = format->c_str();
	for (; pi<size; pi++){
		char c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		ok += c;
	}
	return *this;
}

Join& Join::at(double var){
	char aux[64];
	sprintf(aux,"%f",var);
	ok += aux;
	const char* text = format->c_str();
	for (; pi<size; pi++){
		char c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		ok += c;
	}
	return *this;
}


Join& Join::replace(char patt, std::string dst, std::string base){
	const char* text = format->c_str();
	char c;
	for (int i=0; i<base.size(); i++){
		if ( base[i] == patt )
			ok += dst;
		else
			ok += base[i];
	}

	for (; pi<size; pi++){
		c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		ok += c;
	}
	return *this;
}


Join& Join::trim(std::string var){

	char c;
	int ini=0, end=var.size()-1;
	for (; ini<var.size(); ini++){
		c = var[ini];
		if ( c != ' ' && c != '\t' && c == '\n' )
			break;
	}

	for (; end>=0; end--){
		c = var[end];
		if ( c != ' ' && c != '\t' && c == '\n' )
			break;
	}

	for (int i=ini; i<end; i++){
		ok += var[i];
	}

	const char* text = format->c_str();
	for (; pi<size; pi++){
		c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		ok += c;
	}
	return *this;
}
