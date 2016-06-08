#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>


#include "../include/tiparser.hpp"
#include "../include/tiobj.hpp"

using namespace std;
using namespace  cv;

void teste(string a){
	string res;
	parseFile(res, a);
	cout << res;
}


void teste2(){
	TiObj obj;
	obj.create();
	
	obj["name"] = "Felipe";
	obj.set("idade", 10);
	obj.set("ano", 1.222);
	
	
	TiObj aux;
	aux.create();
	aux["name"] = "Clovis";
	aux["idade"] = 61;
	obj["pai"] = aux;
	
	obj.box() += aux;
	cout << obj;
	
	//TiVet* var = &obj.var();
}



void operator<<(TiObj a, string b){
	a.box() += TiObj(b);
}

void operator<<(TiObj a, Mat& img){
	a.clear();
	a.classe() = "Mat:Cv";
	a.set("rows", img.rows);
	a.set("cols", img.cols);
	a.set("step", (long int)img.step[0]);
	a.set("type", img.type());
	if ( img.type() == CV_8UC1 ){
		//buf.set("type", "char");
		a.set("channels", 1);
	} else if ( img.type() == CV_8UC2 ){
		//buf.set("type", "char");
		a.set("channels", 2);
	} else if ( img.type() == CV_8UC3 ){
		//buf.set("type", "char");
		a.set("channels", 3);
	} else if ( img.type() == CV_32S ){
		//buf.set("type", "int");
		a.set("channels", 1);
	} else if ( img.type() == CV_32F ){
		//buf.set("type", "float");
		a.set("channels", 1);
	}
	a.setBinary("data", img.data, img.rows*img.step[0] );
}





int main(int argc, char** argv){
	TiObj b;

	Mat img;
	img = imread("image.jpg");

	b.create();
	b << img;
	cout << b;



	//string a = argv[1];
	
	/*TiObj obj;
	TiStream stream(stdin);
	while ( stream.next(obj) ){
		//cout << obj << endl;
	}*/
	
	
	/*TiParser parser;
	
	parser.loadFile(stdin);
	parser.output->min = 1;
	
	_TiObj* a = new _TiObj();
	while ( parser.isGood() ){
		parser.parseStream();
		cout << parser.output->text << endl;
		
		a->clear();
		build_tiasm(*a, parser.output->text);
		
		string aux;
		a->encode(aux,0,false,false);
		cout << aux;
	}*/

	//cout << obj.length() << endl;
	//cout << obj.size() << endl;
	//obj.tiasm();
	
	//cout << obj;
	
	//teste(a);
	//teste(a);
	//teste(a);
	//teste(a);

	return 0;
}
