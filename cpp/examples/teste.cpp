#include <iostream>
#include "../include/tiobj.hpp"
#include <string.h>


using namespace std;

#define TISTR_SIZE 4


/*class TiStr {
public:
	char _data[TISTR_SIZE];
	std::string extend;
	int _size;

	TiStr(){
		_size = 0;
	}

	uint size(){
		if ( extend.empty() ){
			return _size;
		} else 
			return extend.size();
	}

	void operator=(std::string txt){
		if ( txt.size() < TISTR_SIZE ){
			strcpy(this->_data,txt.c_str());
		} else {
			extend = txt;
		}
	}

};*/


class TrObj {
	TiObj* ptr;
  public:

	TrObj(){
		this->ptr = new TiObj();
		this->ptr->count_ref += 1;
	}

	~TrObj(){
		this->destroy();
	}

	void operator=(TrObj obj){
		this->destroy();
		this->ptr = obj.ptr;
		this->ptr->count_ref++;
	}

	inline void clear(){this->ptr->clear();}

	inline int  loadText(std::string text){return this->ptr->loadText(text);}


	inline std::string toString(){return this->ptr->toString();}


	void destroy(){
		if ( this->ptr->magic == 0xCAFECAFE ){
			cout << this->ptr->count_ref << endl;
			this->ptr->count_ref -= 1;
			if ( this->ptr->count_ref == 0 )
				delete(this->ptr);
		}
	}
};


void aqui_da_erro(TiObj& saida){
	TiObj* pessoa = new TiObj();
	pessoa->loadText("name='Clovis'");
	saida.set("pai",pessoa);
	saida.box += pessoa;
}

int main(){
	//cout << str.size() << endl;

	TiObj obj;
	obj.loadFile("teste.ti");
	cout << obj.tiasm();

	aqui_da_erro(obj);


	return 0;
}


