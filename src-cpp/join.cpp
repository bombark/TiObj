#include "tiobj.hpp"




Join::Join(std::string format){
	this->format = &format;
	buffer.reserve(256);

	size = format.size();
	for (pi=0; pi<size; pi++){
		char c = format[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		buffer += c;
	}
}

Join& Join::at(std::string var){
	buffer += var;
	const char* text = format->c_str();
	for (; pi<size; pi++){
		char c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		buffer += c;
	}
	return *this;
}


Join& Join::at(int var){
	char aux[64];
	sprintf(aux,"%d",var);
	buffer += aux;
	const char* text = format->c_str();
	for (; pi<size; pi++){
		char c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		buffer += c;
	}
	return *this;
}

Join& Join::at(double var){
	char aux[64];
	sprintf(aux,"%f",var);
	buffer += aux;
	const char* text = format->c_str();
	for (; pi<size; pi++){
		char c = text[pi];
		if ( c == '%' ){
			pi += 2;
			break;
		}
		buffer += c;
	}
	return *this;
}
