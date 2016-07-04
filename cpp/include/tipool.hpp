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



/*=====================================  HEADER  ======================================*/

#pragma once

#include <iostream>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <string.h>

/*-------------------------------------------------------------------------------------*/



/*===================================  TiPoolNode  ====================================*/

template<typename Tipo>
class TiPoolNode {
  public:
	static const int MAX = 64; // dont change

  private:
	size_t id;
	long int status;

  public:
	TiPoolNode(size_t id){
		this->id = id;
		status = -1;
		for (int id=0; id<MAX; id++){
			*this->_at(id) = id;
		}
	}


	/** @brief create a new block with smartpointer counter */
	void* operator new(size_t size){
		TiPoolNode* ptr =  (TiPoolNode*) ::malloc( size + MAX * (sizeof(Tipo)+sizeof(int)) );
		assert(ptr);
		return ptr;
	}

	void operator delete(void *_ptr){
		::free(_ptr);
	}

	/** @brief Get the value in the position pos */
	inline int* _at(size_t pos){
		return (int*)(  ((char*)this) + sizeof(TiPoolNode) + pos*(sizeof(Tipo)+sizeof(int)));
	}

	/** @brief Get the value in the position pos */
	inline Tipo& at(size_t pos){
		Tipo* ptr = (Tipo*) ((char*)this->_at(pos) + sizeof(int));
		return *ptr;
	}

	~TiPoolNode(){
	}

	Tipo* malloc(){
		unsigned char id  = this->getFreeId();
		this->status     &= ~(0x1L << id);
		return &this->at(id);
	}

	void free(unsigned char id){
		this->status |= 0x1L << id;
	}

	static inline unsigned back(unsigned char id){
		return (  sizeof(TiPoolNode) + id*(sizeof(Tipo)+sizeof(int)) + sizeof(int) );
	}

	inline bool hasFreeBlock(){return this->status != 0;}
	inline bool hasnotFreeBlock(){return this->status == 0;}
	inline bool isEmpty(){return this->status == -1L;}

  private:
	unsigned char getFreeId(){
		unsigned char* p_status = (unsigned char*) &this->status;
		for ( int i=0; i<sizeof(this->status); i++){
			unsigned char byte = p_status[i];
			if ( byte == 0 )
				continue;
			unsigned char b_id = i<<3;
			for ( ; byte!=0; byte>>=1, ++b_id){
				if ( (byte&0x1) == 0x1 ){
					return b_id;
				}
			}
		}
	}

};

/*-------------------------------------------------------------------------------------*/



/*=====================================  TiPool  ======================================*/

template<typename Tipo>
class TiPool {
	std::vector< TiPoolNode<Tipo>* > data;
	std::vector< TiPoolNode<Tipo>* > stack_free;

  public:
	TiPool(){
	}

	~TiPool(){
		for (size_t i=0; i<data.size(); i++){
			delete data[i];
		}
		this->data.clear();
		this->stack_free.clear();
	}

	Tipo* malloc(){
		if ( this->stack_free.size() == 0 ){
			TiPoolNode<Tipo>* node = new TiPoolNode<Tipo>( this->data.size() );
			this->data.push_back( node );
			this->stack_free.push_back( node );
		}
		TiPoolNode<Tipo>* node = this->stack_free[this->stack_free.size()-1];
		Tipo* ptr = node->malloc();
		if ( node->hasnotFreeBlock() )
			this->stack_free.pop_back();
		return ptr;
	}

	void free (void* ptr){
		int b_id =  *( ((int*)ptr)-1 );
		TiPoolNode<Tipo>* node = (TiPoolNode<Tipo>*) ((char*) ptr - TiPoolNode<Tipo>::back( b_id ));
		node->free(b_id);
		for (size_t i=0; i<this->stack_free.size(); i++){
			if ( this->stack_free[i] == node )
				return;
		}
		//if ( node->isEmpty() ){

		//} else {
			this->stack_free.push_back(node);
		//}
	}

};

/*-------------------------------------------------------------------------------------*/
