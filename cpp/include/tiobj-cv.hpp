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

#include <opencv2/core/core.hpp>
//#include <tiobj.hpp>

/*-------------------------------------------------------------------------------------*/



/*======================================- TIOBJ -======================================*/


void operator<<(TiObj nom, cv::Mat& akk){
	//nom.clear();
	nom->set("class", "Mat:Cv");
	nom->set("rows", akk.rows);
	nom->set("cols", akk.cols);
	nom->set("step", (long int)akk.step[0]);
	nom->set("type", akk.type());
	if ( akk.type() == CV_8UC1 ){
		//buf.set("type", "char");
		nom->set("channels", 1);
	} else if ( akk.type() == CV_8UC2 ){
		//buf.set("type", "char");
		nom->set("channels", 2);
	} else if ( akk.type() == CV_8UC3 ){
		//buf.set("type", "char");
		nom->set("channels", 3);
	} else if ( akk.type() == CV_32S ){
		//buf.set("type", "int");
		nom->set("channels", 1);
	} else if ( akk.type() == CV_32F ){
		//buf.set("type", "float");
		nom->set("channels", 1);
	}

	nom->setBinary("data", akk.data, akk.rows*akk.step[0] );
}


void operator<<(cv::Mat& nom, TiObj akk){
	size_t   cols = akk->atInt("cols");
	size_t   rows = akk->atInt("rows");
	unsigned type = akk->atInt("type");

	if ( !nom.data ){
		nom.create( rows, cols, type );
	}

	std::string data = akk->atStr("data");
	const char* src = data.c_str();
	for (size_t i=0; i<data.size(); i++){
		nom.at<char>(i) = src[i];
	}
}


/*-------------------------------------------------------------------------------------*/
