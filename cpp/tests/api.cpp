#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <fstream>

#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>


#include "../include/tiobj.hpp"

using namespace std;
using namespace  cv;


volatile size_t G_i = 0;


void* terminator(void* ){
	unsigned total=0;
	unsigned secs = 5;
	for (unsigned i=0; i<secs; i++){
		sleep(1);
		size_t cont = G_i;
		fprintf(stderr, "opaaaa %ld\n", cont);
		total += cont;
		G_i = 0;
	}
	fprintf(stderr, "media: %f\n", total/(double)secs);
	return NULL;
}



#include <sys/stat.h>

int main(int argc, char** argv){
	//TiObj a, b,c;

	/*TiObj b;
	b.load("../tests/var_1000.ti");

	ofstream file;
	file.open ( "tmp/var_1000.ti" );
	file << b.toAsm();
	file.close();*/

	//TiObj b;
	//b.load("../tests/error01.ti");

	//TiObj p("name='felipe';idade=10;score=1.2356");
	//cout << p;

	TiStream stream(stdin);

	TiObj obj;
	while ( stream.next(obj) ){
		cout << obj << endl;
	}


	/*for ( auto node : p->box ){
		cout << node->atStr("name") << endl;
	}*/


	//for( auto var : b ){
	/*for (auto i = b.begin(); i != b.end(); ++i){
		//cout << i.name << endl;
	}*/

	//cout << b;

	/*Mat img;
	img = imread("image.jpg");

	b.create();
	b << img;
	cout << b;*/



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
