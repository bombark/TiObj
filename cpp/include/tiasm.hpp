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

#pragma once
#include <string>

/*-------------------------------------------------------------------------------------*/


/*=====================================- TIASM -=======================================*/

class TiAsm {
  public:
	std::string text;
	int min, now;
	bool is_obj_end;

	TiAsm(int min=0);
	inline void start(){
		text="!T1V";
		this->is_obj_end=false;
		this->now=0;
	}

	// Put Complex
	void printInt(std::string name, long int a);
	void printDbl(std::string name, double a);
	void printStr(std::string name, std::string in);
	void printBin(std::string name, std::string text);
	void printVarObj(std::string name, std::string obj_class);
	void printObj(std::string obj_class);
	void printRet();

	inline bool isObjEnd(){return this->is_obj_end;}

  private:
	// Put Basic
	inline void putCmd(char cmd){
		this->text += cmd;
		this->text += ' ';
	}
	void putInt(long int number);
	void putDbl(double number);
	void putStr(std::string text);
	void putBin(std::string in);
	void putVarName(std::string name);

};

/*-------------------------------------------------------------------------------------*/



